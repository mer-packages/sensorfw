/**
   @file powermanagementtests.cpp
   @brief Automatic tests for power management
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

#include "powermanagementtests.h"
#include <stdlib.h>
#include <QFile>
#include <QTimer>
#include <QProcess>

#define DISABLE_TKLOCK QProcess::execute("gconftool-2 --set /system/osso/dsm/locks/touchscreen_keypad_autolock_enabled --type=bool false");
#define ENABLE_TKLOCK QProcess::execute("gconftool-2 --set /system/osso/dsm/locks/touchscreen_keypad_autolock_enabled --type=bool true");
#define OPEN_TKLOCK QProcess::execute("mcetool --set-tklock-mode=unlocked");
#define BLANK_SCREEN QProcess::execute("mcetool --blank-screen");
#define UNBLANK_SCREEN QProcess::execute("mcetool --unblank-screen");

const char* PowerManagementTest::accPollFile = "/tmp/fakedsensors/accelerometer_poll_rate";
const char* PowerManagementTest::accInputFile = "/tmp/fakedsensors/accelerometer";

unsigned int readPollInterval(QString path)
{
    QFile file(path);
    int returnValue = -1;

    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        char buf[16];
        file.readLine(buf, sizeof(buf));
        file.close();

        returnValue = atoi(buf);
    }

    return (unsigned int)returnValue;
}

void PowerManagementTest::initTestCase()
{
    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
    QVERIFY( remoteSensorManager.isValid() );

    // Load plugins (should test running depend on plug-in load result?)
    remoteSensorManager.loadPlugin("accelerometersensor");

    remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");

    DISABLE_TKLOCK;
    OPEN_TKLOCK;

    helper.setInputFile(accInputFile);
    helper.start();
}

void PowerManagementTest::init() {}
void PowerManagementTest::cleanup() {}
void PowerManagementTest::cleanupTestCase()
{
    ENABLE_TKLOCK;
    helper.stop();
    helper.wait();
}

void PowerManagementTest::testIntervalStartStop()
{
    UNBLANK_SCREEN;

    AccelerometerSensorChannelInterface* accOne = AccelerometerSensorChannelInterface::interface("accelerometersensor");
    QVERIFY2(accOne != NULL && accOne->isValid(), accOne ? accOne->errorString().toLatin1() : "Null pointer");

    // Screen orientation keeps acc open, the 'default' may change.
    // Thus dynamic values.
    unsigned int originalInterval = readPollInterval(accPollFile);
    unsigned int testInterval = originalInterval > 1 ? originalInterval / 2 : 100;

    accOne->setInterval(testInterval);

    QVERIFY(readPollInterval(accPollFile) == originalInterval);

    accOne->start();
    QVERIFY(readPollInterval(accPollFile) == testInterval);

    accOne->stop();

    QVERIFY(readPollInterval(accPollFile) == originalInterval);

    delete accOne;
}

void PowerManagementTest::testIntervalRace()
{
    UNBLANK_SCREEN;

    AccelerometerSensorChannelInterface* accOne = AccelerometerSensorChannelInterface::interface("accelerometersensor");
    QVERIFY2(accOne != NULL && accOne->isValid(), accOne ? accOne->errorString().toLatin1() : "Null pointer");

    AccelerometerSensorChannelInterface* accTwo = AccelerometerSensorChannelInterface::interface("accelerometersensor");
    QVERIFY2(accTwo != NULL && accTwo->isValid(), accTwo ? accTwo->errorString().toLatin1() : "Null pointer");

    // Screen orientation keeps acc open, the 'default' may change.
    // Thus dynamic values.
    unsigned int originalInterval = readPollInterval(accPollFile);
    qDebug() << "original interval:" << originalInterval;
    QVERIFY2((originalInterval == 0 || originalInterval > 3), "Can't run the test with current poll value.");

    unsigned int testIntervalOne = originalInterval > 2 ? originalInterval / 2 : 100; // Faster than original
    unsigned int testIntervalTwo = testIntervalOne / 2; // Faster than previous

    accOne->setInterval(testIntervalOne);
    accOne->start();

    QVERIFY(readPollInterval(accPollFile) == testIntervalOne);

    accTwo->setInterval(testIntervalTwo);
    accTwo->start();

    QVERIFY(readPollInterval(accPollFile) == testIntervalTwo);

    accTwo->stop();

    QVERIFY(readPollInterval(accPollFile) == testIntervalOne);

    // testIntervalTwo*3 is 3/4 of original, thus slower than
    // testIntervalOne, but faster than original.
    accTwo->setInterval(testIntervalTwo*3);
    accTwo->start();

    QVERIFY(readPollInterval(accPollFile) == testIntervalOne);

    accOne->stop();

    QVERIFY(readPollInterval(accPollFile) == testIntervalTwo*3);

    accTwo->stop();

    QVERIFY(readPollInterval(accPollFile) == originalInterval);

    delete accOne;
    delete accTwo;
}

/**
 * Verify normal screen blank behavior.
 */
void PowerManagementTest::testScreenBlank1()
{
    // Unblank to make sure we notice going into blank
    UNBLANK_SCREEN;

    AccelerometerSensorChannelInterface* accOne = AccelerometerSensorChannelInterface::interface("accelerometersensor");
    QVERIFY2(accOne != NULL && accOne->isValid(), accOne ? accOne->errorString().toLatin1() : "Null pointer");

    connect(accOne, SIGNAL(dataAvailable(const XYZ&)), &helper, SLOT(dataAvailable(const XYZ&)));

    accOne->start();

    QTest::qWait(1000);
    QVERIFY2(helper.m_valueCount > 0, "No samples received.");

    // Blank screen
    BLANK_SCREEN;
    QTest::qWait(500);
    helper.reset(); // Clear the buffer
    QTest::qWait(1000);

    // Verify that values have not come through
    QVERIFY2(helper.m_valueCount == 0, "Samples leaking through");

    // Unblank
    UNBLANK_SCREEN;
    QTest::qWait(500);

    // Reset - in testing the old value might stay in the fakepipe.
    helper.reset();

    // Verity that values comes through again
    QTest::qWait(1000);
    QVERIFY(helper.m_valueCount > 0);

    accOne->stop();

    delete accOne;
}

/**
 * Verify stopping sensor while screen is blanked.
 */
void PowerManagementTest::testScreenBlank2()
{
    // Unblank to make sure we notice going into blank
    UNBLANK_SCREEN;

    AccelerometerSensorChannelInterface* accOne = AccelerometerSensorChannelInterface::interface("accelerometersensor");
    QVERIFY2(accOne != NULL && accOne->isValid(), accOne ? accOne->errorString().toLatin1() : "Null pointer");

    connect(accOne, SIGNAL(dataAvailable(const XYZ&)), &helper, SLOT(dataAvailable(const XYZ&)));

    accOne->start();

    QTest::qWait(1000);
    QVERIFY2(helper.m_valueCount > 0, "No samples received.");

    // Blank screen
    BLANK_SCREEN;
    QTest::qWait(500);
    helper.reset(); // Clear the buffer
    QTest::qWait(1000);

    // Stop the sensor
    accOne->stop();

    // Verify that values have not come through
    QVERIFY2(helper.m_valueCount == 0, "Samples leaking through");

    // Unblank
    UNBLANK_SCREEN;
    QTest::qWait(500);

    // Reset - in testing the old value might stay in the fakepipe.
    helper.reset();

    // Verity that sensor was not incorrectly resumed
    QTest::qWait(1000);
    QVERIFY(helper.m_valueCount == 0);

    delete accOne;
}

/**
 * Verify starting sensor while screen is blanked.
 */
void PowerManagementTest::testScreenBlank3()
{
    // Unblank to make sure we notice going into blank
    UNBLANK_SCREEN;
    QTest::qWait(1000);

    AccelerometerSensorChannelInterface* accOne = AccelerometerSensorChannelInterface::interface("accelerometersensor");
    QVERIFY2(accOne != NULL && accOne->isValid(), accOne ? accOne->errorString().toLatin1() : "Null pointer");

    connect(accOne, SIGNAL(dataAvailable(const XYZ&)), &helper, SLOT(dataAvailable(const XYZ&)));

    // Blank screen
    BLANK_SCREEN;
    QTest::qWait(1000);
    helper.reset(); // Clear the buffer

    // Start the sensor
    accOne->start();

    QTest::qWait(1000);

    // Verify that values have not come through
    QVERIFY2(helper.m_valueCount == 0, "Samples leaking through");

    // Unblank
    UNBLANK_SCREEN;
    QTest::qWait(1000);

    // Verify that sensor was correctly resumed
    QTest::qWait(2000);
    QVERIFY2(helper.m_valueCount > 0, "Not receiving samples after resume");

    // Stop the sensor
    accOne->stop();

    delete accOne;
}

/**
 * Verify screen blank override behavior.
 */
void PowerManagementTest::testScreenBlank4()
{
    // Unblank to make sure we notice going into blank
    UNBLANK_SCREEN;
    QTest::qWait(1000);

    AccelerometerSensorChannelInterface* accOne = AccelerometerSensorChannelInterface::interface("accelerometersensor");
    QVERIFY2(accOne != NULL && accOne->isValid(), accOne ? accOne->errorString().toLatin1() : "Null pointer");

    connect(accOne, SIGNAL(dataAvailable(const XYZ&)), &helper, SLOT(dataAvailable(const XYZ&)));

    accOne->setStandbyOverride(true);
    accOne->start();

    QTest::qWait(1000);
    QVERIFY2(helper.m_valueCount > 0, "No samples received.");

    // Blank screen
    BLANK_SCREEN;
    QTest::qWait(1000);
    helper.reset(); // Clear the buffer
    QTest::qWait(1000);

    // Verify that values have not come through
    QVERIFY2(helper.m_valueCount >  0, "Adaptor apparently went to standby against rules");

    // Unblank
    UNBLANK_SCREEN;
    QTest::qWait(1000);

    // Reset - in testing the old value might stay in the fakepipe.
    helper.reset();

    // Verity that values comes through again
    QTest::qWait(1000);
    QVERIFY2(helper.m_valueCount > 0, "Not getting values after resume.");

    accOne->stop();

    delete accOne;
}

QTEST_MAIN(PowerManagementTest)
