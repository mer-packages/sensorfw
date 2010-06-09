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

#include "qt-api/sensormanagerinterface.h"
#include "qt-api/accelerometersensor_i.h"

#include "metadatatest.h"


void MetaDataTest::initTestCase()
{
    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
    QVERIFY( remoteSensorManager.isValid() );

    // Load plugins (should test running depend on plug-in load result?)
    remoteSensorManager.loadPlugin("accelerometersensor");

    remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");
}

void MetaDataTest::init()
{
    //qDebug() << "Run before each test";
    //TODO: Verify that sensord has not crashed.
}

void MetaDataTest::cleanup()
{
    //qDebug() << "Run after each test";
    //TODO: Verify that sensord has not crashed.
}

void MetaDataTest::cleanupTestCase()
{
    //qDebug() << "Run after all test cases";
}

void MetaDataTest::testDescription()
{
    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

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

    // Get control session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

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

    // Get control session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

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

void MetaDataTest::testConcurrentRangeRequests()
{
    // Test behavior of two different sessions making concurrent requests
}

void MetaDataTest::testChangeNotifications()
{
    DummyHelper dummy;

    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Request list of available data ranges
    QList<DataRange> dataRangeList = sensorIfc->getAvailableDataRanges();
    QVERIFY2(dataRangeList.size() > 0, "No data ranges received from sensor");

    // Connect change notification signal
    connect(sensorIfc, SIGNAL(propertyChanged(const QString&)), &dummy, SLOT(propertyChanged(const QString&)));

    // Make some requests
    sensorIfc->requestDataRange(dataRangeList.last());
    sensorIfc->start();
    sensorIfc->setInterval(50);
    sensorIfc->stop();

    // Allow signals some time ...
    QTest::qWait(200);

    delete sensorIfc;

    QVERIFY2(dummy.hasReceived("datarange"), "Property notification for \"DataRange\" not received");
    QVERIFY2(dummy.hasReceived("interval"), "Property notification for \"Interval\" not received");
}

void MetaDataTest::testAvailableIntervals()
{
    // TODO: This type of test should be done for each sensor separately

    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Request list of available intervals
    QList<DataRange> intervalList = sensorIfc->getAvailableIntervals();

    delete sensorIfc;

    QVERIFY2(intervalList.size() > 0, "No intervals received from sensor");
}

QTEST_MAIN(MetaDataTest)
