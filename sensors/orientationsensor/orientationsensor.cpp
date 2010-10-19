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

    orientationChain_ = sm.requestChain("orientationchain");
    Q_ASSERT( orientationChain_ );
    if (!orientationChain_->isValid()) {
        isValid_ = false;
    } else {
        isValid_ = true;
    }


    orientationReader_ = new BufferReader<PoseData>(128);

    outputBuffer_ = new RingBuffer<PoseData>(128);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(orientationReader_, "orientation");
    filterBin_->add(outputBuffer_, "buffer");

    // Join filterchain buffers
    filterBin_->join("orientation", "source", "buffer", "sink");
    filterBin_->join("face", "source", "buffer", "sink");

    // Join datasources to the chain
    connectToSource(orientationChain_, "orientation", orientationReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    setDescription("orientation of the device screen as 6 pre-defined positions");
    setRangeSource(orientationChain_);
    addStandbyOverrideSource(orientationChain_);
    setIntervalSource(orientationChain_);
}

OrientationSensorChannel::~OrientationSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    disconnectFromSource(orientationChain_, "orientation", orientationReader_);

    sm.releaseChain("orientationchain");

    delete orientationReader_;
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
        orientationChain_->start();
    }
    return true;
}

bool OrientationSensorChannel::stop()
{
    sensordLogD() << "Stopping OrientationSensorChannel";

    if (AbstractSensorChannel::stop()) {
        orientationChain_->stop();
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void OrientationSensorChannel::emitToDbus(const PoseData& value)
{
    if ((value.orientation_ != prevOrientation.orientation_) &&
        (value.orientation_ != PoseData::Undefined) )  {
        prevOrientation.orientation_ = value.orientation_;
        writeToClients((const void *)&value, sizeof(value));
    }
}
