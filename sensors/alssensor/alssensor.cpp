/**
   @file alssensor.cpp
   @brief ALSSensor

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

#include "alssensor.h"

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"
#include "datatypes/orientation.h"

#ifdef PROVIDE_CONTEXT_INFO
#include "serviceinfo.h"
#endif

ALSSensorChannel::ALSSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DataEmitter<TimedUnsigned>(10),
        previousValue_(0,0)
#ifdef PROVIDE_CONTEXT_INFO
        ,service(QDBusConnection::systemBus()),
        isDarkProperty(service, "Environment.IsDark"),
        isBrightProperty(service, "Environment.IsBright")
#endif
{
    SensorManager& sm = SensorManager::instance();

    alsAdaptor_ = sm.requestDeviceAdaptor("alsadaptor");
    Q_ASSERT( alsAdaptor_ );

    alsReader_ = new BufferReader<TimedUnsigned>(32);

    outputBuffer_ = new RingBuffer<TimedUnsigned>(32);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(alsReader_, "als");
    filterBin_->add(outputBuffer_, "buffer");

    filterBin_->join("als", "source", "buffer", "sink");

    // Join datasources to the chain
    connectToSource(alsAdaptor_, "als", alsReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

#ifdef PROVIDE_CONTEXT_INFO
    // Start listening to context clients. When a client comes, we
    // start the sensor channel, and when we no more have clients, we
    // stop it.
    propertyGroup.add(isDarkProperty);
    propertyGroup.add(isBrightProperty);
    connect(&propertyGroup, SIGNAL(firstSubscriberAppeared()), this, SLOT(start()));
    connect(&propertyGroup, SIGNAL(lastSubscriberDisappeared()), this, SLOT(stop()));
    // Note: We should combine this "service needed / not needed"
    // information from Context FW with information from other
    // sources. This doesn't do it yet.

#endif

    setDescription("ambient light intensity in lux");
    setRangeSource(alsAdaptor_);
    addStandbyOverrideSource(alsAdaptor_);
    setIntervalSource(alsAdaptor_);

    setValid(true);
}

ALSSensorChannel::~ALSSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    disconnectFromSource(alsAdaptor_, "als", alsReader_);

    sm.releaseDeviceAdaptor("alsadaptor");

    delete alsReader_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

bool ALSSensorChannel::start()
{
    sensordLogD() << "Starting ALSSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        alsAdaptor_->startSensor();
    }
    return true;
}

bool ALSSensorChannel::stop()
{
    sensordLogD() << "Stopping ALSSensorChannel";

    if (AbstractSensorChannel::stop()) {
        alsAdaptor_->stopSensor();
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void ALSSensorChannel::emitToDbus(const TimedUnsigned& value)
{
    if (value.value_ != previousValue_.value_) {
        previousValue_.value_ = value.value_;

        writeToClients((const void*)(&value), sizeof(value));
    }

#ifdef PROVIDE_CONTEXT_INFO
    // Publish the new data via Context FW. Note that setting the same
    // value twice does no harm.
    if (value.value_ < 10) {
        isDarkProperty.setValue(true);
        isBrightProperty.setValue(false);
    }
    else if (value.value_ > 290) {
        isBrightProperty.setValue(true);
        isDarkProperty.setValue(false);
    }
    else{
        isDarkProperty.setValue(false);
        isBrightProperty.setValue(false);
    }
#endif
}
