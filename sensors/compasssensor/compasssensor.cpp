/**
   @file compasssensor.cpp
   @brief CompassSensor

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

#include "compasssensor.h"
#include "sensord/sensormanager.h"
#include "sensord/bin.h"
#include "sensord/bufferreader.h"
#include "logging.h"

CompassSensorChannel::CompassSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DbusEmitter<CompassData>(10),
        declinationCorrection_(false),
        declinationAngle_(0),
        compassData(0, -1, -1)
{
    SensorManager& sm = SensorManager::instance();

    isValid_ = true;

    compassChain_ = sm.requestChain("compasschain");
    Q_ASSERT( compassChain_ );
    if (!compassChain_->isValid()) {
        isValid_ = false;
    }

    inputReader_ = new BufferReader<CompassData>(10);

    outputBuffer_ = new RingBuffer<CompassData>(1024);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(inputReader_, "input");
    filterBin_->add(outputBuffer_, "output");

    // Join filterchain buffers
    filterBin_->join("input", "source", "output", "sink");

    // Pick input depending on declination correctin value (to func)
    RingBufferBase* rb;
    rb = compassChain_->findBuffer("magneticnorth");
    Q_ASSERT(rb);
    rb->join(inputReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    setDescription("compass north in degrees");

    // Enlist used adaptors
    adaptorList_ << "accelerometeradaptor" << "magnetometeradaptor" << "kbslideradaptor";

    introduceAvailableDataRange(DataRange(0, 359, 1));

    // Interval limited to 5hz - 1000hz (theoretical). Keeps calibration
    // sane.
    intervalList_.append(DataRange(1, 200, 0));
}

CompassSensorChannel::~CompassSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    // TODO: Put this to separate function
    RingBufferBase* rb;
    if (declinationCorrection_) {
        rb = compassChain_->findBuffer("truenorth");
    } else {
        rb = compassChain_->findBuffer("magneticnorth");
    }
    Q_ASSERT(rb);

    rb->unjoin(inputReader_);
    sm.releaseChain("compasschain");

    delete inputReader_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

void CompassSensorChannel::setDeclination(bool enable) {
    RingBufferBase* rb = NULL;

    if ( enable == declinationCorrection_ )
        return;

    //select the correct output buffer from CompassChain.
    if ( enable ) {
        rb = compassChain_->findBuffer("magneticnorth");
        Q_ASSERT(rb);
        rb->unjoin(inputReader_);
        
        rb = compassChain_->findBuffer("truenorth");
        Q_ASSERT(rb);
        rb->join(inputReader_);
    } else {
        rb = compassChain_->findBuffer("truenorth");
        Q_ASSERT(rb);
        rb->unjoin(inputReader_);
        
        rb = compassChain_->findBuffer("magneticnorth");
        Q_ASSERT(rb);
        rb->join(inputReader_);
    }

    declinationCorrection_ = enable;
    signalPropertyChanged("usedeclination");
}

quint16 CompassSensorChannel::declinationAngle() { 
    return qvariant_cast< int >
        (compassChain_->property("declinationvalue"));
}

bool CompassSensorChannel::start()
{
    sensordLogD() << "Starting CompassSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        compassChain_->setProperty("compassEnabled", true);
        compassChain_->start();
    }
    return true;
}

bool CompassSensorChannel::stop()
{
    sensordLogD() << "Stopping CompassSensorChannel";

    if (AbstractSensorChannel::stop()) {
        compassChain_->stop();
        compassChain_->setProperty("compassEnabled", false);
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void CompassSensorChannel::emitToDbus(const CompassData& value)
{
    compassData.timestamp_ = value.timestamp_;
    compassData.level_ = value.level_;
    compassData.degrees_ = value.degrees_;

#ifdef USE_SOCKET
    writeToClients((const void*)(&value), sizeof(CompassData));
#else
    emit dataAvailable(value);
#endif
}

int CompassSensorChannel::interval() const
{
    /* Compass pace is defined by accelerometer output. */
    return SensorManager::instance().propertyHandler().getHighestValue("interval", "accelerometeradaptor");
}
