/**
   @file dataflowtests.cpp
   @brief Automatic tests for data flow
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Pia Niemela <pia.s.niemela@nokia.com>

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

#include <QtDebug>
#include <QTest>
#include <QVariant>

#include <typeinfo>
#include <sensord/sensormanager.h>
#include "sensord/bin.h"
#include "sensord/bufferreader.h"
#include "sensord/filter.h"
#include "sensord/config.h"
#include "dataflowtests.h"
#include "loader.h"
#include "plugin.h"
#include <accelerometeradaptor/accelerometeradaptor.h>
#include <accelerometerchain/accelerometerchain.h>
#include <coordinatealignfilter/coordinatealignfilter.h>

#include <stdio.h>
#include <stdlib.h>
class TestSensorManager2;

void DataFlowTest::initTestCase()
{
    Config::loadConfig("/etc/sensorfw/sensord.conf", "/etc/sensorfw/sensord.conf.d");

    // Initialise sensormanager
    SensorManager& sm = SensorManager::instance();
    Q_UNUSED(sm);
}

void DataFlowTest::cleanupTestCase()
{
}

void DataFlowTest::testAdaptorSharing()
{
    SensorManager& sm = SensorManager::instance();

    QString adaptorString("accelerometeradaptor");
    QString sensorName("accelerometer");

    QCOMPARE(sm.loadPlugin(adaptorString), true);

    DeviceAdaptor* adaptorA = NULL;
    DeviceAdaptor* adaptorB = NULL;

    // Request A
    adaptorA = sm.requestDeviceAdaptor(adaptorString);
    QVERIFY(adaptorA);

    // Request B
    adaptorB = sm.requestDeviceAdaptor(adaptorString);
    QVERIFY(adaptorB);

    // Check that A = B
    QVERIFY2(adaptorA == adaptorB, "Adaptors are not the same instance");

    // start A
    QVERIFY(adaptorA->startSensor(sensorName));

    // Check that A runs, is not running unless sensor is started
    SysfsAdaptor* sysfsAdaptorA = dynamic_cast<SysfsAdaptor*>(adaptorA);
    QVERIFY2(sysfsAdaptorA->isRunning(), "AdaptorA is not running after start");

    // start B
    QVERIFY(adaptorB->startSensor(sensorName));

    // Check that B runs, is not running unless sensor is started

    SysfsAdaptor* sysfsAdaptorB = dynamic_cast<SysfsAdaptor*>(adaptorB);
    QVERIFY2(sysfsAdaptorB->isRunning(), "AdaptorB is not running after start");

    // Stop B
    adaptorB->stopSensor(sensorName);

    // Check that A still runs
    QVERIFY2(sysfsAdaptorA->isRunning(), "AdaptorA is not running after stopping adaptor B");

    // TODO: Should we make some buffer tests (probably, startAdaptor()
    //       is likely to be empty).

    // Stop A
    adaptorA->stopSensor(sensorName);

    // check that A stopped
    QVERIFY2(!(sysfsAdaptorA->isRunning()), "AdaptorA is still running after stopping both adaptor A and B");

    // Release once
    sm.releaseDeviceAdaptor(adaptorString);

    // Check that the adaptor still exists
    // TestSensorManager - check ref count
    int adaptorCount = TestSensorManager2::getAdaptorCount(sm, adaptorString);
    QString msg("After releasing there should be one adaptor left, however had ");
    QString number;
    msg.append(number.setNum(adaptorCount));
    QVERIFY2(adaptorCount==1, msg.toLocal8Bit().constData());

    int refCount = TestSensorManager2::getRefCountSum(sm);
    msg = "After releasing reference count should equal to 1, however was ";
    msg.append(number.setNum(refCount));
    QVERIFY2(refCount==1, msg.toLocal8Bit().constData());

    // release second time
    sm.releaseDeviceAdaptor(adaptorString);

    // check that the adaptor was destroyed
    adaptorCount = TestSensorManager2::getAdaptorCount(sm, adaptorString);

    // check that the adaptor was destroyed
    // TestSensorManager - check ref count
    msg = " After releasing twice there should be no adaptors left, however had ";
    msg.append(number.setNum(adaptorCount));
    QVERIFY2(adaptorCount==0 , msg.toLocal8Bit().constData());
}

void DataFlowTest::testChainSharing()
{
    SensorManager& sm = SensorManager::instance();

    QCOMPARE(sm.loadPlugin("accelerometerchain"), true);

    AbstractChain* chainA = NULL;
    AbstractChain* chainB = NULL;

    // Request A
    chainA = sm.requestChain("accelerometerchain");
    QVERIFY( chainA );

    // Request B
    chainB = sm.requestChain("accelerometerchain");
    QVERIFY( chainB );

    // Check that A = B
    QVERIFY2(chainA == chainB, "Chains are not the same instance");

    // start A
    QVERIFY(chainA->start());
    QVERIFY(chainA->running());

    // start B
    QVERIFY(chainB->start());
    QVERIFY(chainB->running());

    // Stop B
    QVERIFY(chainB->stop());
    QVERIFY(chainB->running());

    // Stop A
    QVERIFY(chainA->stop());
    QVERIFY(!(chainA->running()));

    // release once
    sm.releaseChain("accelerometerchain");

    // Check that chain still exists
    // TODO: Must see inside sensord...

    // release second time
    sm.releaseChain("accelerometerchain");
    // check that does not exist
}

QTEST_MAIN(DataFlowTest)
