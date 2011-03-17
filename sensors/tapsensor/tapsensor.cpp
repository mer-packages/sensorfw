/**
   @file tapsensor.cpp
   @brief TapSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#include "tapsensor.h"

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"
#include "datatypes/tap.h"

TapSensorChannel::TapSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DataEmitter<TapData>(1)
{
    SensorManager& sm = SensorManager::instance();

    tapAdaptor_ = sm.requestDeviceAdaptor("tapadaptor");
    Q_ASSERT( tapAdaptor_ );

    tapReader_ = new BufferReader<TapData>(1);

    outputBuffer_ = new RingBuffer<TapData>(1);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(tapReader_, "tap");
    filterBin_->add(outputBuffer_, "buffer");

    filterBin_->join("tap", "source", "buffer", "sink");

    // Join datasources to the chain
    connectToSource(tapAdaptor_, "tap", tapReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    setValid(true);

    setDescription("either single or double device taps, and tap axis");
    setRangeSource(tapAdaptor_);
    setIntervalSource(tapAdaptor_);

    // Tap needs to work with display off
    addStandbyOverrideSource(tapAdaptor_);
}

TapSensorChannel::~TapSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    disconnectFromSource(tapAdaptor_, "tap", tapReader_);
    sm.releaseDeviceAdaptor("tapadaptor");

    delete tapReader_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

bool TapSensorChannel::start()
{
    sensordLogD() << "Starting TapSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        tapAdaptor_->startSensor("tap");
    }
    return true;
}

bool TapSensorChannel::stop()
{
    sensordLogD() << "Stopping TapSensorChannel";

    if (AbstractSensorChannel::stop()) {
        tapAdaptor_->stopSensor("tap");
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void TapSensorChannel::emitData(const TapData& tapData)
{
    writeToClients((const void *)&tapData, sizeof(TapData));
}
