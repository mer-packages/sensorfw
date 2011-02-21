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

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"

RotationSensorChannel::RotationSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DataEmitter<TimedXyzData>(10),
        prevRotation_(0,0,0,0),
        hasZ_(false)
{
    SensorManager& sm = SensorManager::instance();

    accelerometerChain_ = sm.requestChain("accelerometerchain");
    Q_ASSERT( accelerometerChain_ );
    setValid(accelerometerChain_->isValid());

    accelerometerReader_ = new BufferReader<AccelerationData>(1);

    compassChain_ = sm.requestChain("compasschain");
    if (compassChain_ && compassChain_->isValid()) {
        hasZ_ = true;
        compassReader_ = new BufferReader<CompassData>(1);
    } else {
        sensordLogW() << "Unable to use compass for z-axis rotation.";
    }

    rotationFilter_ = sm.instantiateFilter("rotationfilter");
    Q_ASSERT(rotationFilter_);

    outputBuffer_ = new RingBuffer<TimedXyzData>(1);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(accelerometerReader_, "accelerometer");
    filterBin_->add(rotationFilter_, "rotationfilter");
    filterBin_->add(outputBuffer_, "buffer");

    if (hasZ_)
    {
        filterBin_->add(compassReader_, "compass");
        filterBin_->join("compass", "source", "rotationfilter", "compasssink");
    }

    filterBin_->join("accelerometer", "source", "rotationfilter", "accelerometersink");
    filterBin_->join("rotationfilter", "source", "buffer", "sink");

    connectToSource(accelerometerChain_, "accelerometer", accelerometerReader_);

    if (hasZ_)
    {
        connectToSource(compassChain_, "truenorth", compassReader_);
        addStandbyOverrideSource(compassChain_);
    }

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    setDescription("x, y, and z axes rotation in degrees");
    introduceAvailableDataRange(DataRange(-179, 180, 1));
    addStandbyOverrideSource(accelerometerChain_);

    // Provide interval value from acc, but range depends on sane compass
    if (hasZ_)
    {
        // No less than 5hz allowed for compass
        introduceAvailableInterval(DataRange(10, 200, 0));  //-> [5-100] Hz
    } else {
        setIntervalSource(accelerometerChain_);
    }

    setDefaultInterval(100); // Tricky. Might need to make this conditional.
}

RotationSensorChannel::~RotationSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    disconnectFromSource(accelerometerChain_, "accelerometer", accelerometerReader_);
    sm.releaseChain("accelerometerchain");

    if (hasZ_)
    {
        disconnectFromSource(compassChain_, "truenorth", compassReader_);
        sm.releaseChain("compasschain");
        delete compassReader_;
    }

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
        if (hasZ_)
        {
            compassChain_->setProperty("compassEnabled", true);
            compassChain_->start();
        }
    }
    return true;
}

bool RotationSensorChannel::stop()
{
    sensordLogD() << "Stopping RotationSensorChannel";

    if (AbstractSensorChannel::stop()) {
        accelerometerChain_->stop();
        filterBin_->stop();
        if (hasZ_)
        {
            compassChain_->stop();
            compassChain_->setProperty("compassEnabled", false);
        }
        marshallingBin_->stop();
    }
    return true;
}

void RotationSensorChannel::emitData(const TimedXyzData& value)
{
    prevRotation_ = value;
    writeToClients((const void*)(&value), sizeof(TimedXyzData));
}

unsigned int RotationSensorChannel::interval() const
{
    // Just provide ACC rate as a kludge for now.
    // This is not precise, as the actual rate depends on both.
    return accelerometerChain_->getInterval();
}

bool RotationSensorChannel::setInterval(unsigned int value, int sessionId)
{
    bool success = accelerometerChain_->setIntervalRequest(sessionId, value);
    if (hasZ_)
    {
        success = compassChain_->setIntervalRequest(sessionId, value) && success;
    }

    return success;
}
