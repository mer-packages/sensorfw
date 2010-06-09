/**
   @file magnetometersensor.cpp
   @brief MagnetometerSensor

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

#include "magnetometersensor.h"

#include "sensord/sensormanager.h"
#include "sensord/bin.h"
#include "sensord/bufferreader.h"

MagnetometerSensorChannel::MagnetometerSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DbusEmitter<CalibratedMagneticFieldData>(10),
        prevMeasurement_()
{
    isValid_ = true;

    SensorManager& sm = SensorManager::instance();

    compassChain_ = sm.requestChain("compasschain");
    Q_ASSERT( compassChain_ );
    if (!compassChain_->isValid()) {
        isValid_ = false;
    }

    magnetometerReader_ = new BufferReader<CalibratedMagneticFieldData>(1024);

    outputBuffer_ = new RingBuffer<CalibratedMagneticFieldData>(1024);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(magnetometerReader_, "magnetometer");
    filterBin_->add(outputBuffer_, "buffer");

    filterBin_->join("magnetometer", "source", "buffer", "sink");

    // Join datasources to the chain
    RingBufferBase* rb;
    rb = compassChain_->findBuffer("calibratedmagnetometerdata");
    Q_ASSERT(rb);
    rb->join(magnetometerReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    // Set sensor description
    description_ = "magnetic flux density in teslas";

    // Enlist used adaptors
    adaptorList_ << "magnetometeradaptor" << "kbslideradaptor";

    // List possible data ranges
    // TODO: Figure out correct datarange
    dataRangeList_.append(DataRange(-2048, 2048, 1));
    intervalList_.append(DataRange(0, 100000, 0));
}

MagnetometerSensorChannel::~MagnetometerSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    RingBufferBase* rb;
    rb = compassChain_->findBuffer("calibratedmagnetometerdata");
    Q_ASSERT(rb);
    rb->unjoin(magnetometerReader_);
    sm.releaseChain("compasschain");
 
    delete magnetometerReader_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

bool MagnetometerSensorChannel::start()
{
    sensordLogD() << "Starting MagnetometerSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        compassChain_->start();
    }
    return true;
}

bool MagnetometerSensorChannel::stop()
{
    sensordLogD() << "Stopping MagnetometerSensorChannel";

    if (AbstractSensorChannel::stop()) {
        compassChain_->stop();
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void MagnetometerSensorChannel::emitToDbus(const CalibratedMagneticFieldData& value)
{
    prevMeasurement_ = value; // TODO: Does this need locking?
#ifdef USE_SOCKET
    writeToClients((const void*)(&value), sizeof(CalibratedMagneticFieldData));
    emit internalData(value);
#else
    //~ TimedXyzData tmpValue(value.timestamp_, value.calibratedData.x_, value.calibratedData.y_, value.calibratedData.z_);
    emit dataAvailable(value);
#endif
}

void MagnetometerSensorChannel::reset_(int dummy)
{
    Q_UNUSED(dummy);

    if (!isValid_)
        return;

    QObject *cc = dynamic_cast<QObject*>(compassChain_);
    compassChain_->setProperty("resetCalibration",0);
}
