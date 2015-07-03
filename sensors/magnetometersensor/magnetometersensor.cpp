/**
   @file magnetometersensor.cpp
   @brief MagnetometerSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"
#include "config.h"

MagnetometerSensorChannel::MagnetometerSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DataEmitter<CalibratedMagneticFieldData>(1),
        scaleFilter_(NULL),
        prevMeasurement_()
{
    SensorManager& sm = SensorManager::instance();

    magChain_ = sm.requestChain("magcalibrationchain");
    if (!magChain_) {
        setValid(false);
        return;
    }
    setValid(magChain_->isValid());

    magnetometerReader_ = new BufferReader<CalibratedMagneticFieldData>(1);

    scaleCoefficient_ = Config::configuration()->value("magnetometer/scale_coefficient", QVariant(300)).toInt();

    if (scaleCoefficient_ != 1)
    {
        scaleFilter_ = sm.instantiateFilter("magnetometerscalefilter");
        if (scaleFilter_ == NULL)
        {
            sensordLogW() << "Failed to initialise scaling filter for magnetometer.";
        }
    }

    outputBuffer_ = new RingBuffer<CalibratedMagneticFieldData>(1);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(magnetometerReader_, "magnetometer");
    filterBin_->add(outputBuffer_, "buffer");

    if (scaleFilter_)
    {
        filterBin_->add(scaleFilter_, "scaleFilter");

        if (!filterBin_->join("magnetometer", "source", "scaleFilter", "sink"))
            qDebug() << Q_FUNC_INFO << "magnetometer/scaleFilter join failed";

        if (!filterBin_->join("filter", "source", "buffer", "sink"))
            qDebug() << Q_FUNC_INFO << "source/buffer join failed";

    } else
    {
        if (!filterBin_->join("magnetometer", "source", "buffer", "sink"))
            qDebug() << Q_FUNC_INFO << "magnetometer/buffer join failed";
    }

    // Join datasources to the chain
    connectToSource(magChain_, "calibratedmagnetometerdata", magnetometerReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    // AK897X requires scaling, which affects available ranges
    if (scaleFilter_)
    {
        // Get available ranges and introduce modified ones
        QList<DataRange> rangeList = magChain_->getAvailableDataRanges();
        foreach(const DataRange& range, rangeList)
        {
            introduceAvailableDataRange(DataRange(scaleCoefficient_*range.min, scaleCoefficient_*range.max, scaleCoefficient_*range.resolution));
        }
    } else {
        // Use the ranges directly from source
        setRangeSource(magChain_);
    }

    setDescription("magnetic flux density in nT");
    addStandbyOverrideSource(magChain_);
    setIntervalSource(magChain_);
}

MagnetometerSensorChannel::~MagnetometerSensorChannel()
{
    if (isValid()) {
        SensorManager& sm = SensorManager::instance();

        disconnectFromSource(magChain_, "calibratedmagnetometerdata", magnetometerReader_);
        sm.releaseChain("magcalibrationchain");

        if (scaleFilter_) delete scaleFilter_;

        delete magnetometerReader_;
        delete outputBuffer_;
        delete marshallingBin_;
        delete filterBin_;
    }
}

bool MagnetometerSensorChannel::start()
{
    sensordLogD() << "Starting MagnetometerSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        magChain_->start();
    }
    return true;
}

bool MagnetometerSensorChannel::stop()
{
    sensordLogD() << "Stopping MagnetometerSensorChannel";

    if (AbstractSensorChannel::stop()) {
        magChain_->stop();
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

void MagnetometerSensorChannel::emitData(const CalibratedMagneticFieldData& value)
{
    prevMeasurement_ = value;
    downsampleAndPropagate(value, downsampleBuffer_);
    emit internalData(value);
}

void MagnetometerSensorChannel::resetCalibration()
{
    if (!magChain_)
        return;
    QMetaObject::invokeMethod(magChain_, "resetCalibration", Qt::DirectConnection);
}

bool MagnetometerSensorChannel::setDataRange(const DataRange& range, int sessionId)
{
    DataRange request;
    request.min = range.min * scaleCoefficient_;
    request.max = range.max * scaleCoefficient_;
    request.resolution = range.resolution * scaleCoefficient_;

    magChain_->requestDataRange(sessionId, request);
    return true;
}

void MagnetometerSensorChannel::removeSession(int sessionId)
{
    downsampleBuffer_.remove(sessionId);
    AbstractSensorChannel::removeSession(sessionId);
}

bool MagnetometerSensorChannel::downsamplingSupported() const
{
    return true;
}
