/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com

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
 */

#include <QVariant>
#include <QStringList>
#include <QDebug>

#include "magcalibrationchain.h"
#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"
#include "config.h"
#include "logging.h"
#include "calibrationfilter.h"

//#include "coordinatealignfilter.h"
#include "datatypes/orientationdata.h"
// magcalibrationchain requires: magnetometeradaptor, kbslideradaptor

MagCalibrationChain::MagCalibrationChain(const QString& id) :
    AbstractChain(id)
{
    qDebug() << Q_FUNC_INFO << id;

    SensorManager& sm = SensorManager::instance();

    magAdaptor = sm.requestDeviceAdaptor("magnetometeradaptor");
    setValid(magAdaptor->isValid());

// Config::configuration()->value<int>("magnetometer/interval_compensation", 16);
    magReader = new BufferReader<TimedXyzData>(1);

    magCalFilter = sm.instantiateFilter("calibrationfilter");
    magScaleFilter = sm.instantiateFilter("magnetometerscalefilter");


    calibratedMagnetometerData = new RingBuffer<CalibratedMagneticFieldData>(1);
    nameOutputBuffer("calibratedmagnetometerdata", calibratedMagnetometerData);

    // Create buffers for filter chain
    filterBin = new Bin;
//formationsink
    filterBin->add(magReader, "calibratedmagneticfield");
    filterBin->add(magCalFilter, "calibration");
    filterBin->add(magScaleFilter,"filter");

    filterBin->add(calibratedMagnetometerData, "calibratedmagnetometerdata"); //calibration

    // Join filterchain buffers
    if (!filterBin->join("calibratedmagneticfield", "source", "calibration", "magsink"))
        qDebug() << Q_FUNC_INFO << "calibratedmagneticfield join failed";

    if (!filterBin->join("calibration", "calibratedmagneticfield", "calibratedmagnetometerdata", "sink"))
        qDebug() << Q_FUNC_INFO << "calibration join failed";


//////    ///////
//    if (!filterBin->join("magnetometeradaptor", "source", "filter", "sink"))
//        qDebug() << Q_FUNC_INFO << "magnetometer join failed";

//    if (!filterBin->join("filter", "source", "buffer", "sink"))
//         qDebug() << Q_FUNC_INFO << "filter join failed";


    // Join datasources to the chain
    connectToSource(magAdaptor, "calibratedmagneticfield", magReader);

    setDescription("Calibrated Mag values"); //Magnetometer calibration
    setRangeSource(magAdaptor);
    addStandbyOverrideSource(magAdaptor);
    setIntervalSource(magAdaptor);
}

MagCalibrationChain::~MagCalibrationChain()
{
    SensorManager& sm = SensorManager::instance();

    disconnectFromSource(magAdaptor, "magnetometer", magReader);

    sm.releaseDeviceAdaptor("magnetometeradaptor");

    delete magReader;
    delete magCalFilter;
    delete calibratedMagnetometerData;
    delete filterBin;
}

bool MagCalibrationChain::start()
{
    if (AbstractSensorChannel::start()) {
        sensordLogD() << "Starting MagCalibrationChain";
        filterBin->start();
        magAdaptor->startSensor();
    }
    return true;
}

bool MagCalibrationChain::stop()
{
    if (AbstractSensorChannel::stop()) {
        sensordLogD() << "Stopping MagCalibrationChain";
        magAdaptor->stopSensor();
        filterBin->stop();
    }
    return true;
}

void MagCalibrationChain::resetCalibration()
{
    CalibrationFilter *filter = static_cast<CalibrationFilter *>(magCalFilter);
    filter->dropCalibration();
    qDebug() << Q_FUNC_INFO;
}
