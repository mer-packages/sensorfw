/**
   @file rotationsensor.cpp
   @brief RotationSensor

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

#include "rotationsensor.h"

#include "sensord/sensormanager.h"
#include "sensord/bin.h"
#include "sensord/bufferreader.h"

RotationSensorChannel::RotationSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DbusEmitter<TimedXyzData>(10),
        prevRotation_(0,0,0,0)
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

    rotationFilter_ = sm.instantiateFilter("rotationfilter");
    Q_ASSERT(rotationFilter_);

    outputBuffer_ = new RingBuffer<TimedXyzData>(1024);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(accelerometerReader_, "accelerometer");
    filterBin_->add(rotationFilter_, "rotationfilter");
    filterBin_->add(outputBuffer_, "buffer");

    filterBin_->join("accelerometer", "source", "rotationfilter", "sink");
    filterBin_->join("rotationfilter", "source", "buffer", "sink");
    
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

RotationSensorChannel::~RotationSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    RingBufferBase* rb;
    rb = accelerometerChain_->findBuffer("accelerometer");
    Q_ASSERT(rb);
    rb->unjoin(accelerometerReader_);

    sm.releaseChain("accelerometerchain");
 
    delete accelerometerReader_;
    delete rotationFilter_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

bool RotationSensorChannel::start()
{
    sensordLogD() << "Starting RotationSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        accelerometerChain_->start();
    }
    return true;
}

bool RotationSensorChannel::stop()
{
    sensordLogD() << "Stopping RotationSensorChannel";

    if (AbstractSensorChannel::stop()) {
        accelerometerChain_->stop();
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void RotationSensorChannel::emitToDbus(const TimedXyzData& value)
{
    prevRotation_ = value; // TODO: Does this need locking?
#ifdef USE_SOCKET
    writeToClients((const void*)(&value), sizeof(TimedXyzData));
#else
    emit dataAvailable(value);
#endif
}
