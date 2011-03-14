/**
   @file accelerometersensor.cpp
   @brief AccelerometerSensor

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

#include "accelerometersensor.h"

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"

AccelerometerSensorChannel::AccelerometerSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DataEmitter<AccelerationData>(10),
        previousSample_(0,0,0,0)
{
    SensorManager& sm = SensorManager::instance();

    accelerometerChain_ = sm.requestChain("accelerometerchain");
    Q_ASSERT( accelerometerChain_ );
    setValid(accelerometerChain_->isValid());

    accelerometerReader_ = new BufferReader<AccelerationData>(1);

    outputBuffer_ = new RingBuffer<AccelerationData>(1);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(accelerometerReader_, "accelerometer");
    filterBin_->add(outputBuffer_, "buffer");

    filterBin_->join("accelerometer", "source", "buffer", "sink");

    // Join datasources to the chain
    connectToSource(accelerometerChain_, "accelerometer", accelerometerReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    // Set MetaData
    setDescription("x, y, and z axes accelerations in mG");
    setRangeSource(accelerometerChain_);
    addStandbyOverrideSource(accelerometerChain_);
    setIntervalSource(accelerometerChain_);
}

AccelerometerSensorChannel::~AccelerometerSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    disconnectFromSource(accelerometerChain_, "accelerometer", accelerometerReader_);

    sm.releaseChain("accelerometerchain");

    delete accelerometerReader_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

bool AccelerometerSensorChannel::start()
{
    sensordLogD() << "Starting AccelerometerSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        accelerometerChain_->start();
    }
    return true;
}

bool AccelerometerSensorChannel::stop()
{
    sensordLogD() << "Stopping AccelerometerSensorChannel";

    if (AbstractSensorChannel::stop()) {
        accelerometerChain_->stop();
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void AccelerometerSensorChannel::emitData(const AccelerationData& value)
{
    previousSample_ = value;
    downsampleAndPropagate(value, downsampleBuffer_);
}

void AccelerometerSensorChannel::removeSession(int sessionId)
{
    downsampleBuffer_.take(sessionId);
    AbstractSensorChannel::removeSession(sessionId);
}

bool AccelerometerSensorChannel::downsamplingSupported() const
{
    return true;
}
