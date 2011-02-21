/**
   @file gyroscopesensor.cpp
   @brief GyroscopeSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>

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

#include "gyroscopesensor.h"

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"

GyroscopeSensorChannel::GyroscopeSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DbusEmitter<AngularVelocityData>(10),
        previousSample_(0,0,0,0)
{
    SensorManager& sm = SensorManager::instance();

    gyroscopeAdaptor_ = sm.requestDeviceAdaptor("gyroscopeadaptor");
    Q_ASSERT( gyroscopeAdaptor_ );

    gyroscopeReader_ = new BufferReader<AngularVelocityData>(1);

    outputBuffer_ = new RingBuffer<AngularVelocityData>(1);

    // Create buffers for filter chain
    filterBin_ = new Bin;
    filterBin_->add(gyroscopeReader_, "gyroscope");
    filterBin_->add(outputBuffer_, "output");

    filterBin_->join("gyroscope", "source", "output", "sink");

    // Join datasources to the chain
    connectToSource(gyroscopeAdaptor_, "gyroscope", gyroscopeReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    // Set MetaData
    setDescription("x, y, and z axes angular velocity in mdps");
    setRangeSource(gyroscopeAdaptor_);
    addStandbyOverrideSource(gyroscopeAdaptor_);
    setIntervalSource(gyroscopeAdaptor_);

    setValid(true);
}

GyroscopeSensorChannel::~GyroscopeSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    disconnectFromSource(gyroscopeAdaptor_, "gyroscope", gyroscopeReader_);

    sm.releaseDeviceAdaptor("gyroscopeadaptor");

    delete gyroscopeReader_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

bool GyroscopeSensorChannel::start()
{
    sensordLogD() << "Starting GyroscopeSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        gyroscopeAdaptor_->startSensor("gyroscope");
    }
    return true;
}

bool GyroscopeSensorChannel::stop()
{
    sensordLogD() << "Stopping GyroscopeSensorChannel";

    if (AbstractSensorChannel::stop()) {
        gyroscopeAdaptor_->stopSensor("gyroscope");
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void GyroscopeSensorChannel::emitToDbus(const AngularVelocityData& value)
{
    previousSample_ = value;
    writeToClients((const void*)(&value), sizeof(AngularVelocityData));
}
