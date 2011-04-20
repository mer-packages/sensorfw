/**
   @file metadatatest.cpp
   @brief Automatic tests for sensor client interfaces

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

#include "sensormanagerinterface.h"
#include "accelerometersensor_i.h"
#include "magnetometersensor_i.h"
#include "alssensor_i.h"
#include "compasssensor_i.h"
#include "rotationsensor_i.h"
#include "tapsensor_i.h"
#include "proximitysensor_i.h"
#include "orientationsensor_i.h"

#include "metadatatest.h"

void MetaDataTest::initTestCase()
{
    qDBusRegisterMetaType<IntegerRange>();
    qDBusRegisterMetaType<IntegerRangeList>();
    qDBusRegisterMetaType<DataRange>();
    qDBusRegisterMetaType<DataRangeList>();

    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
    QVERIFY( remoteSensorManager.isValid() );

    // Load plugins (should test running depend on plug-in load result?)
    remoteSensorManager.loadPlugin("accelerometersensor");
    remoteSensorManager.loadPlugin("magnetometersensor");
    remoteSensorManager.loadPlugin("alssensor");
    remoteSensorManager.loadPlugin("orientationsensor");
    remoteSensorManager.loadPlugin("tapsensor");
    remoteSensorManager.loadPlugin("rotationsensor");
    remoteSensorManager.loadPlugin("compasssensor");
    remoteSensorManager.loadPlugin("proximitysensor");

    remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");
    remoteSensorManager.registerSensorInterface<MagnetometerSensorChannelInterface>("magnetometersensor");
    remoteSensorManager.registerSensorInterface<ALSSensorChannelInterface>("alssensor");
    remoteSensorManager.registerSensorInterface<OrientationSensorChannelInterface>("orientationsensor");
    remoteSensorManager.registerSensorInterface<TapSensorChannelInterface>("tapsensor");
    remoteSensorManager.registerSensorInterface<RotationSensorChannelInterface>("rotationsensor");
    remoteSensorManager.registerSensorInterface<CompassSensorChannelInterface>("compasssensor");
    remoteSensorManager.registerSensorInterface<ProximitySensorChannelInterface>("proximitysensor");
}

void MetaDataTest::init()
{
}

void MetaDataTest::cleanup()
{
}

void MetaDataTest::cleanupTestCase()
{
}

void MetaDataTest::testDescription()
{
    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    QString desc = sensorIfc->description();

    QVERIFY2(desc.size() > 0, "No description received for sensor");
    qDebug() << sensorName << "reported description: " << desc;

    delete sensorIfc;
}

void MetaDataTest::testAvailableRanges()
{
    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Request list of available data ranges
    QList<DataRange> dataRangeList = sensorIfc->getAvailableDataRanges();
    QVERIFY2(dataRangeList.size() > 0, "No data ranges received from sensor");
    qDebug() << "Received ranges:";
    for (int i = 0; i < dataRangeList.size(); i++) {
        DataRange range = dataRangeList.at(i);
        qDebug() << i+1 << ". [" << range.min << ", " << range.max << "], " << range.resolution;
    }

    delete sensorIfc;
}

void MetaDataTest::testGetCurrentRange()
{
    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Request list of available data ranges
    QList<DataRange> dataRangeList = sensorIfc->getAvailableDataRanges();
    QVERIFY2(dataRangeList.size() > 0, "No data ranges received from sensor");

    // Verify that default == the first
    DataRange range = sensorIfc->getCurrentDataRange();
    QVERIFY2(range == dataRangeList.at(0), "Default range is not the first option.");

    // Request a different range
    sensorIfc->requestDataRange(dataRangeList.last());

    // Verify that current is the requested one (needs at least 2 range options)
    range = sensorIfc->getCurrentDataRange();
    QVERIFY2(range == dataRangeList.last() || dataRangeList.size() < 2, "Requested rate not set correctly.");

    // Remove request
    sensorIfc->removeDataRangeRequest();

    // Verify that current == default (needs at least 2 range options)
    range = sensorIfc->getCurrentDataRange();
    QVERIFY2(range == dataRangeList.at(0) || dataRangeList.size() < 2, "Default range is not the first option.");

    delete sensorIfc;
}

void MetaDataTest::testChangeNotifications()
{
    DummyHelper dummy;

    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Request list of available data ranges
    QList<DataRange> dataRangeList = sensorIfc->getAvailableDataRanges();
    QVERIFY2(dataRangeList.size() > 0, "No data ranges received from sensor");

    if (dataRangeList.size() > 1)
    {
        // Connect change notification signal
        connect(sensorIfc, SIGNAL(propertyChanged(const QString&)), &dummy, SLOT(propertyChanged(const QString&)));

        // Make some requests
        sensorIfc->requestDataRange(dataRangeList.last());
        sensorIfc->start();
        sensorIfc->setInterval(50);
        sensorIfc->stop();

        // Allow signals some time ...
        QTest::qWait(200);

        QVERIFY2(dummy.hasReceived("datarange"), "Property notification for \"DataRange\" not received");
        QVERIFY2(dummy.hasReceived("interval"), "Property notification for \"Interval\" not received");
    }

    delete sensorIfc;
}

void MetaDataTest::testAvailableIntervals()
{
    // TODO: This type of test should be done for each sensor separately

    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Request list of available intervals
    QList<DataRange> intervalList = sensorIfc->getAvailableIntervals();

    delete sensorIfc;

    QVERIFY2(intervalList.size() > 0, "No intervals received from sensor");
}

void MetaDataTest::testAvailableBufferIntervals()
{
    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Request list of available intervals
    IntegerRangeList intervalList = sensorIfc->getAvailableBufferIntervals();

    delete sensorIfc;

    QVERIFY2(intervalList.size() > 0, "No buffer intervals received from sensor");
}

void MetaDataTest::testAvailableBufferSizes()
{
    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Request list of available intervals
    IntegerRangeList sizeList = sensorIfc->getAvailableBufferSizes();

    delete sensorIfc;

    QVERIFY2(sizeList.size() > 0, "No buffer sizes received from sensor");
}

void MetaDataTest::testCompassDeclination()
{
    CompassSensorChannelInterface* compass = CompassSensorChannelInterface::interface("compasssensor");
    compass->start();
    system("gconftool-2 --set /system/osso/location/settings/magneticvariation --type int 50");
    QCOMPARE(compass->declinationValue(), 50);
    system("gconftool-2 --set /system/osso/location/settings/magneticvariation --type int 0");
    QCOMPARE(compass->declinationValue(), 0);
    compass->stop();
    delete compass;
}

void MetaDataTest::printMetaData()
{
    QList<QString> sensorNameList;
    QList<AbstractSensorChannelInterface*> sensorList;

    // Initiate sensors
    sensorNameList << "accelerometer";
    sensorList << const_cast<AccelerometerSensorChannelInterface*>(AccelerometerSensorChannelInterface::interface("accelerometersensor"));

    sensorNameList << "magnetometer";
    sensorList << const_cast<MagnetometerSensorChannelInterface*>(MagnetometerSensorChannelInterface::interface("magnetometersensor"));

    sensorNameList << "als";
    sensorList << const_cast<ALSSensorChannelInterface*>(ALSSensorChannelInterface::interface("alssensor"));

    sensorNameList << "proximity";
    sensorList << const_cast<ProximitySensorChannelInterface*>(ProximitySensorChannelInterface::interface("proximitysensor"));

    sensorNameList << "orientation";
    sensorList << const_cast<OrientationSensorChannelInterface*>(OrientationSensorChannelInterface::interface("orientationsensor"));

    sensorNameList << "tap";
    sensorList << const_cast<TapSensorChannelInterface*>(TapSensorChannelInterface::interface("tapsensor"));

    sensorNameList << "compass";
    sensorList << const_cast<CompassSensorChannelInterface*>(CompassSensorChannelInterface::interface("compasssensor"));

    sensorNameList << "rotation";
    sensorList << const_cast<RotationSensorChannelInterface*>(RotationSensorChannelInterface::interface("rotationsensor"));

    AbstractSensorChannelInterface* sensor;
    while (sensorList.size() > 0)
    {
        qDebug() << "[" << sensorNameList.takeFirst() << "]:";
        sensor = sensorList.takeFirst();
        if (sensor != NULL)
        {
            qDebug() << "   Description    :" << sensor->description().toAscii().data();

            qDebug() << "   Interval       :" << sensor->interval();
            qDebug() << "   Possible rates :";
            QList<DataRange> rateList = sensor->getAvailableIntervals();
            QString msg = "Interval setting failed for the sensor ";
            msg.append(sensor->id());
            msg.append(" interval was ");
            for (int i = 1; i <= rateList.size(); i++)
            {
                DataRange r = rateList.at(i-1);
                if (r.min == r.max)
                {
                    qDebug() << QString("                    %1. %2").arg(i).arg(r.min).toAscii().data();
                } else {
                    qDebug() << QString("                    %1. [%2, %3]").arg(i).arg(r.min).arg(r.max).toAscii().data();
                }

                // min
                sensor->setInterval(r.min);
                QVERIFY2(r.min==sensor->interval(),msg.append(QString::number(r.min)).toAscii());
                // max
                sensor->setInterval(r.max);
                QVERIFY2(r.max==sensor->interval(),msg.append(QString::number(r.max)).toAscii());
                // average
                int value = (r.min+r.max)*0.5;
                sensor->setInterval(value);
                QVERIFY2(value==sensor->interval(),msg.append(QString::number(value)).toAscii());
            }


            DataRange r = sensor->getCurrentDataRange();
            qDebug() << QString("   Data Range     : [%1, %2], resolution %3").arg(r.min).arg(r.max).arg(r.resolution).toAscii().data();
            qDebug() << "   Possible ranges:";
            QList<DataRange> rangeList = sensor->getAvailableDataRanges();
            for (int i = 1; i <= rangeList.size(); i++)
            {
                r = rangeList.at(i-1);
                qDebug() << QString("                    %1. [%2, %3], resolution %4").arg(i).arg(r.min).arg(r.max).arg(r.resolution).toAscii().data();
            }

            delete sensor;
        } else {
            qDebug() << "   <sensor initialisation failed>";
        }
        qDebug() << "";
    }
}

QTEST_MAIN(MetaDataTest)
