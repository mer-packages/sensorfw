/**
   @file orientationsensor.cpp
   @brief OrientationSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
*/

#include "orientationsensor.h"

#include "sensord/sensormanager.h"
#include "sensord/bin.h"
#include "sensord/bufferreader.h"

OrientationSensorChannel::OrientationSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DbusEmitter<PoseData>(10),
        prevOrientation(PoseData::Undefined)
{
    SensorManager& sm = SensorManager::instance();
    
    accelerometerChain_ = sm.requestChain("accelerometerchain");
    Q_ASSERT( accelerometerChain_ );
    if (!accelerometerChain_->isValid()) {
        isValid_ = false;
    } else {
        isValid_ = true;
    }


    accelerometerReader_ = new BufferReader<AccelerationData>(1024);

    orientationInterpreterFilter_ = sm.instantiateFilter("orientationinterpreter");
    Q_ASSERT(orientationInterpreterFilter_);
    
    outputBuffer_ = new RingBuffer<PoseData>(1024);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(accelerometerReader_, "accelerometer");
    filterBin_->add(orientationInterpreterFilter_, "orientationinterpreter");
    filterBin_->add(outputBuffer_, "buffer");

    // Join filterchain buffers
    filterBin_->join("accelerometer", "source", "orientationinterpreter", "sink");
    filterBin_->join("orientationinterpreter", "source", "buffer", "sink");

    // Join datasources to the chain
    RingBufferBase* rb;
    rb = accelerometerChain_->findBuffer("accelerometer");
    Q_ASSERT(rb);
    rb->join(accelerometerReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    /// Enlist used adaptors
    adaptorList_ << "accelerometeradaptor";
}

OrientationSensorChannel::~OrientationSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    RingBufferBase* rb;
    rb = accelerometerChain_->findBuffer("accelerometer");
    Q_ASSERT(rb);
    rb->unjoin(accelerometerReader_);

    sm.releaseChain("accelerometerchain");

    delete accelerometerReader_;
    delete orientationInterpreterFilter_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

bool OrientationSensorChannel::start()
{
    sensordLogD() << "Starting OrientationSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        accelerometerChain_->start();
    }
    return true;
}

bool OrientationSensorChannel::stop()
{
    sensordLogD() << "Stopping OrientationSensorChannel";

    if (AbstractSensorChannel::stop()) {
        accelerometerChain_->stop();
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void OrientationSensorChannel::emitToDbus(const PoseData& value)
{
    if (value.orientation_ != prevOrientation.orientation_) {
        prevOrientation.orientation_ = value.orientation_;
#ifdef USE_SOCKET
        writeToClients((const void *)&value, sizeof(value));
#else
        emit orientationChanged(TimedUnsigned(value.timestamp_, value.orientation_));
#endif
    }
}
