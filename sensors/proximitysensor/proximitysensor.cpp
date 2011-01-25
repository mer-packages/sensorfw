/**
   @file proximitysensor.cpp
   @brief ProximitySensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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

#include "proximitysensor.h"

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"

ProximitySensorChannel::ProximitySensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DbusEmitter<TimedUnsigned>(10),
        previousValue_(0, -1)
{
    SensorManager& sm = SensorManager::instance();

    proximityAdaptor_ = sm.requestDeviceAdaptor("proximityadaptor");
    Q_ASSERT( proximityAdaptor_ );

    proximityReader_ = new BufferReader<TimedUnsigned>(16);

    outputBuffer_ = new RingBuffer<TimedUnsigned>(16);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(proximityReader_, "proximity");
    filterBin_->add(outputBuffer_, "buffer");

    filterBin_->join("proximity", "source", "buffer", "sink");

    // Join datasources to the chain
    connectToSource(proximityAdaptor_, "proximity", proximityReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    setValid(true);

    setDescription("whether an object is close to device screen");
    setRangeSource(proximityAdaptor_);
    addStandbyOverrideSource(proximityAdaptor_);
    setIntervalSource(proximityAdaptor_);
}

ProximitySensorChannel::~ProximitySensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    disconnectFromSource(proximityAdaptor_, "proximity", proximityReader_);

    sm.releaseDeviceAdaptor("proximityadaptor");

    delete proximityReader_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

bool ProximitySensorChannel::start()
{
    sensordLogD() << "Starting ProximitySensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        proximityAdaptor_->startSensor("proximity");
    }
    return true;
}

bool ProximitySensorChannel::stop()
{
    sensordLogD() << "Stopping ProximitySensorChannel";

    if (AbstractSensorChannel::stop()) {
        proximityAdaptor_->stopSensor("proximity");
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void ProximitySensorChannel::emitToDbus(const TimedUnsigned& value)
{
    previousValue_.timestamp_ = value.timestamp_;
    
    if (value.value_ != previousValue_.value_) {
        previousValue_.value_ = value.value_;
        writeToClients((const void *)&value, sizeof(TimedUnsigned));
    }
}
