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

#include "compasschain.h"
#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"
#include "config.h"
#include "logging.h"

#include "datatypes/orientationdata.h"


CompassChain::CompassChain(const QString& id) :
    AbstractChain(id)
{
    qDebug() << Q_FUNC_INFO << id;

    SensorManager& sm = SensorManager::instance();

    magChain = sm.requestChain("magcalibrationchain");
    Q_ASSERT(magChain);
    setValid(magChain->isValid());

//    orientAdaptor = sm.requestDeviceAdaptor("orientationadaptor");
//    Q_ASSERT(orientAdaptor);
  //  setValid(orientAdaptor->isValid());

    accelerometerChain = sm.requestChain("accelerometerchain");
    Q_ASSERT(accelerometerChain);
    setValid(accelerometerChain->isValid());

    accelerometerReader = new BufferReader<AccelerationData>(1);

    magReader = new BufferReader<CalibratedMagneticFieldData>(1);

//    if (orientAdaptor->isValid())
//        orientationdataReader = new BufferReader<TimedXyzData>(1);

    compassFilter = sm.instantiateFilter("compassfilter");
    Q_ASSERT(compassFilter);

    declinationFilter = sm.instantiateFilter("declinationfilter");
    Q_ASSERT(declinationFilter);

    downsampleFilter = sm.instantiateFilter("downsamplefilter");
    Q_ASSERT(downsampleFilter);

    avgaccFilter = sm.instantiateFilter("avgaccfilter");
    Q_ASSERT(avgaccFilter);

    trueNorthBuffer = new RingBuffer<CompassData>(1);
    nameOutputBuffer("truenorth", trueNorthBuffer); //

    magneticNorthBuffer = new RingBuffer<CompassData>(1);
    nameOutputBuffer("magneticnorth", magneticNorthBuffer); //

    // Create buffers for filter chain
    filterBin = new Bin;

 //   if (orientAdaptor->isValid())
 //       filterBin->add(orientationdataReader, "orientation");

    filterBin->add(magReader, "magnetometer");
    filterBin->add(accelerometerReader, "accelerometer");

    filterBin->add(compassFilter, "compass");
    filterBin->add(avgaccFilter, "avgaccelerometer"); //normalize to flat ?
    filterBin->add(downsampleFilter, "downsamplefilter");
    filterBin->add(declinationFilter, "declinationcorrection");

    filterBin->add(trueNorthBuffer, "truenorth");
    filterBin->add(magneticNorthBuffer, "magneticnorth");

  //  if (orientAdaptor->isValid()) { //hybris has compass heading in orientation sensor
//        if (!filterBin->join("orientation", "source", "compass", "orientsink"))
//            qDebug() << Q_FUNC_INFO << "magnetometer join failed";
  //  } else {

        if (!filterBin->join("magnetometer", "source", "compass", "magsink"))
            qDebug() << Q_FUNC_INFO << "magnetometer join failed";

        if (!filterBin->join("accelerometer", "source", "avgaccelerometer", "sink"))
            qDebug() << Q_FUNC_INFO << "accelerometer join failed";


        if (!filterBin->join("avgaccelerometer", "source", "downsamplefilter", "sink"))
            qDebug() << Q_FUNC_INFO << "avgaccelerometer join failed";

        if (!filterBin->join("downsamplefilter", "source", "compass", "accsink"))
            qDebug() << Q_FUNC_INFO << "downsamplefilter join failed";
  //  }

    if (!filterBin->join("compass", "magnorthangle", "magneticnorth", "sink"))
        qDebug() << Q_FUNC_INFO << "compass1 join failed";

    if (!filterBin->join("compass", "magnorthangle", "declinationcorrection", "sink"))
        qDebug() << Q_FUNC_INFO << "compass2 join failed";

    if (!filterBin->join("declinationcorrection", "source", "truenorth", "sink"))
        qDebug() << Q_FUNC_INFO << "declinationfilter join failed";



    if (!connectToSource(accelerometerChain, "accelerometer", accelerometerReader))
        qDebug() << Q_FUNC_INFO << "accelerometer connect failed";

    if (!connectToSource(magChain, "calibratedmagnetometerdata", magReader))
        qDebug() << Q_FUNC_INFO << "magnetometer connect failed";

//    if (orientAdaptor->isValid()) {
//        if (!connectToSource(orientAdaptor, "orientation", orientationdataReader))
//            qDebug() << Q_FUNC_INFO << "orientation connect failed";
//    }

    setDescription("Compass direction"); //compass north in degrees
    introduceAvailableDataRange(DataRange(0, 359, 1));
    introduceAvailableInterval(DataRange(50,200,0));

    setRangeSource(magChain);
    addStandbyOverrideSource(magChain);

    addStandbyOverrideSource(accelerometerChain);
    setIntervalSource(accelerometerChain);
}

CompassChain::~CompassChain()
{
    SensorManager& sm = SensorManager::instance();

    disconnectFromSource(accelerometerChain, "accelerometer", accelerometerReader);

    disconnectFromSource(magChain, "magnetometer", magReader);

//    disconnectFromSource(orientAdaptor, "orientation", orientationdataReader);
//    sm.releaseDeviceAdaptor("orientationadaptor");

    delete accelerometerReader;
    delete magReader;
//    delete orientationdataReader;
    delete declinationFilter;
    delete trueNorthBuffer;
    delete magneticNorthBuffer;
    delete filterBin;
}

bool CompassChain::start()
{
    if (AbstractSensorChannel::start()) {
        sensordLogD() << "Starting compassChain";
        filterBin->start();
//        if (orientAdaptor->isValid()) {
     //       orientAdaptor->startSensor();
//        } else {
            accelerometerChain->start();
            magChain->start();
//        }
    }
    return true;
}

bool CompassChain::stop()
{
    if (AbstractSensorChannel::stop()) {
        sensordLogD() << "Stopping compassChain";
//        if (orientAdaptor->isValid()) {
     //       orientAdaptor->stopSensor();
//        } else {
            accelerometerChain->stop();
            magChain->stop();
//        }
        filterBin->stop();
    }
    return true;
}

bool CompassChain::compassEnabled() const
{
    return true;
}

void CompassChain::setCompassEnabled(bool enabled)
{
}

quint16 CompassChain::declinationValue() const
{
    return 0;
}

void CompassChain::resetCalibration()
{
    qDebug() << Q_FUNC_INFO;
}
