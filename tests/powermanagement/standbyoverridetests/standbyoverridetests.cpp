/**
   @file standbyoverridetests.cpp
   @brief Automatic tests for standby override
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lhan Guo <lihan.guo@digia.com>

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

#include "standbyoverridetests.h"
#include <stdlib.h>
#include <QFile>
#include <QTimer>
#include <QProcess>
#include <QDebug>

#define DISABLE_TKLOCK QProcess::execute("gconftool-2 --set /system/osso/dsm/locks/touchscreen_keypad_autolock_enabled --type=bool false");
#define ENABLE_TKLOCK QProcess::execute("gconftool-2 --set /system/osso/dsm/locks/touchscreen_keypad_autolock_enabled --type=bool true");
#define OPEN_TKLOCK QProcess::execute("mcetool --set-tklock-mode=unlocked");
#define BLANK_SCREEN QProcess::execute("mcetool --blank-screen");
#define UNBLANK_SCREEN QProcess::execute("mcetool --unblank-screen");

void StandbyOverrideTest::initTestCase()
{
    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
    QVERIFY( remoteSensorManager.isValid() );

    // Load plugins (should test running depend on plug-in load result?)
    remoteSensorManager.loadPlugin("accelerometersensor");

    remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");

    DISABLE_TKLOCK;
    OPEN_TKLOCK;

    helper1.setInputFile(accInputFile);
    helper1.start();
    helper2.start();

}

void StandbyOverrideTest::init() {}
void StandbyOverrideTest::cleanup() {}
void StandbyOverrideTest::cleanupTestCase()
{
    ENABLE_TKLOCK;
    helper1.terminate();
    helper2.terminate();
    helper1.wait();
    helper2.wait();
}

/**
 * Verify standby override for screen blank and unblank.
 */
void StandbyOverrideTest::testStandbyOverride()
{

    UNBLANK_SCREEN;

    AccelerometerSensorChannelInterface* accOne = AccelerometerSensorChannelInterface::controlInterface("accelerometersensor");
    QVERIFY2(accOne != NULL && accOne->isValid(), accOne ? accOne->errorString().toLatin1() : "Null pointer");

    AccelerometerSensorChannelInterface* accTwo = const_cast<AccelerometerSensorChannelInterface*>(AccelerometerSensorChannelInterface::listenInterface("accelerometersensor"));
    QVERIFY2(accTwo != NULL && accTwo->isValid(), accTwo ? accTwo->errorString().toLatin1() : "Null pointer");

    connect(accOne, SIGNAL(dataAvailable(const XYZ&)), &helper1, SLOT(dataAvailable(const XYZ&)));
    connect(accTwo, SIGNAL(dataAvailable(const XYZ&)), &helper2, SLOT(dataAvailable(const XYZ&)));

    accOne->start();
    accTwo->start();

    QTest::qWait(500);

    // Test standby override true when screen unblank
    accOne->setStandbyOverride(true);
    accTwo->setStandbyOverride(false);
    QTest::qWait(500);
    helper1.reset();  // Clear the buffer
    helper2.reset();
    QTest::qWait(1500);

    QVERIFY2(helper1.m_valueCount > 0, "No samples received.");
    QVERIFY2(helper2.m_valueCount > 0, "No samples received.");

    accOne->setStandbyOverride(false);
    QTest::qWait(500);
    helper1.reset();
    helper2.reset();
    QTest::qWait(1500);
    QVERIFY2(helper1.m_valueCount > 0, "No samples received.");
    QVERIFY2(helper2.m_valueCount > 0, "No samples received.");

    // Test standby override when screen blank
    BLANK_SCREEN;
    QTest::qWait(500);
    helper1.reset();
    helper2.reset();
    QTest::qWait(1500);

    QVERIFY2(helper1.m_valueCount == 0, "Samples leaking through.");
    QVERIFY2(helper2.m_valueCount == 0, "Samples leaking through.");

    accOne->setStandbyOverride(true);
    QTest::qWait(500);
    helper1.reset();
    helper2.reset();
    QTest::qWait(1500);

    QVERIFY2(helper1.m_valueCount > 0, "No samples received.");
    QVERIFY2(helper2.m_valueCount > 0, "No samples received.");

    accOne->setStandbyOverride(false);
    QTest::qWait(500);
    helper1.reset();
    helper2.reset();
    QTest::qWait(500);
    QVERIFY2(helper1.m_valueCount == 0, "Samples leaking through.");
    QVERIFY2(helper2.m_valueCount == 0, "Samples leaking through.");

    accOne->setStandbyOverride(true);
    QTest::qWait(500);
    helper1.reset();
    helper2.reset();
    QTest::qWait(1500);
    QVERIFY2(helper1.m_valueCount > 0, "No samples received.");
    QVERIFY2(helper2.m_valueCount > 0, "No samples received.");

    accOne->stop();
    QTest::qWait(500);
    helper1.reset();
    helper2.reset();
    QTest::qWait(1500);
    QVERIFY2(helper1.m_valueCount == 0, "Samples leaking through.");
    QVERIFY2(helper2.m_valueCount == 0, "Samples leaking through.");

    accTwo->stop();

    delete accOne;
    delete accTwo;
}

QTEST_MAIN(StandbyOverrideTest)
