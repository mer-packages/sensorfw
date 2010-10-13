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
#include "sensord/config.h"

MagnetometerSensorChannel::MagnetometerSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DbusEmitter<CalibratedMagneticFieldData>(10),
        prevMeasurement_(),
        prevRangeRequestId_(-1)
{
    isValid_ = true;

    SensorManager& sm = SensorManager::instance();

    compassChain_ = sm.requestChain("compasschain");
    Q_ASSERT( compassChain_ );
    if (!compassChain_->isValid()) {
        isValid_ = false;
    }

    magnetometerReader_ = new BufferReader<CalibratedMagneticFieldData>(128);

    scaleFilter_ = NULL;
    scaleCoefficient_ = Config::configuration()->value("magnetometer_scale_coefficient", QVariant(300)).toInt();

    if (scaleCoefficient_ != 1)
    {
        scaleFilter_ = sm.instantiateFilter("magnetometerscalefilter");
        if (scaleFilter_ == NULL)
        {
            sensordLogW() << "Failed to initialise scaling filter for magnetometer.";
        }
    }

    outputBuffer_ = new RingBuffer<CalibratedMagneticFieldData>(128);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(magnetometerReader_, "magnetometer");
    filterBin_->add(outputBuffer_, "buffer");

    if (scaleFilter_)
    {
        filterBin_->add(scaleFilter_, "filter");
        filterBin_->join("magnetometer", "source", "filter", "sink");
        filterBin_->join("filter", "source", "buffer", "sink");
    } else
    {
        filterBin_->join("magnetometer", "source", "buffer", "sink");
    }

    // Join datasources to the chain
    RingBufferBase* rb;
    rb = compassChain_->findBuffer("calibratedmagnetometerdata");
    Q_ASSERT(rb);
    rb->join(magnetometerReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    setDescription("magnetic flux density in nT");

    // Enlist used adaptors
    adaptorList_ << "magnetometeradaptor" << "kbslideradaptor";

    // AK897X requires scaling, which affects available ranges
    if (scaleFilter_)
    {
        // Get available ranges and introduce modified ones
        QList<DataRange> rangeList = compassChain_->getAvailableDataRanges();
        foreach(DataRange range, rangeList)
        {
            introduceAvailableDataRange(DataRange(scaleCoefficient_*range.min, scaleCoefficient_*range.max, scaleCoefficient_*range.resolution));
        }
    } else {
        // Use the ranges directly from source
        setRangeSource(compassChain_);
    }

    intervalList_.append(DataRange(0, 100000, 0));
    addStandbyOverrideSource(compassChain_);
}

MagnetometerSensorChannel::~MagnetometerSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    RingBufferBase* rb;
    rb = compassChain_->findBuffer("calibratedmagnetometerdata");
    Q_ASSERT(rb);
    rb->unjoin(magnetometerReader_);
    sm.releaseChain("compasschain");

    if (scaleFilter_) delete scaleFilter_;

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
    writeToClients((const void*)(&value), sizeof(CalibratedMagneticFieldData));
    emit internalData(value);
}

void MagnetometerSensorChannel::reset_(int dummy)
{
    Q_UNUSED(dummy);

    if (!isValid_)
        return;

    QObject *cc = dynamic_cast<QObject*>(compassChain_);
    Q_UNUSED(cc); // Cleanup or document why this is done like this
    compassChain_->setProperty("resetCalibration",0);
}

bool MagnetometerSensorChannel::setDataRange(const DataRange range, const int sessionId)
{
    if (sessionId < 0)
    {
        compassChain_->removeDataRangeRequest(prevRangeRequestId_);
        prevRangeRequestId_ = -1;
    } else {
        DataRange request;
        request.min = range.min*scaleCoefficient_;
        request.max = range.max*scaleCoefficient_;
        request.resolution = range.resolution*scaleCoefficient_;

        compassChain_->requestDataRange(sessionId, request);
        prevRangeRequestId_ = sessionId;
    }
    return true;
}
