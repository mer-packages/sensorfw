/**
   @file clientapitest.cpp
   @brief Automatic tests for sensor client interfaces

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

#include "qt-api/sensormanagerinterface.h"

#include "qt-api/orientationsensor_i.h"
#include "qt-api/accelerometersensor_i.h"
#include "qt-api/compasssensor_i.h"
#include "qt-api/tapsensor_i.h"
#include "qt-api/alssensor_i.h"
#include "qt-api/proximitysensor_i.h"
#include "qt-api/rotationsensor_i.h"
#include "qt-api/magnetometersensor_i.h"

#include "clientapitest.h"


void ClientApiTest::initTestCase()
{
    qDBusRegisterMetaType<XYZ>();
    qDBusRegisterMetaType<Compass>();
    qDBusRegisterMetaType<MagneticField>();
    qDBusRegisterMetaType<Tap>();
    qDBusRegisterMetaType<Unsigned>();

    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
    QVERIFY( remoteSensorManager.isValid() );

    // Load plugins (should test running depend on plug-in load result?)
    qDebug() << remoteSensorManager.loadPlugin("orientationsensor");
    remoteSensorManager.loadPlugin("accelerometersensor");
    remoteSensorManager.loadPlugin("compasssensor");
    remoteSensorManager.loadPlugin("tapsensor");
    remoteSensorManager.loadPlugin("alssensor");
    remoteSensorManager.loadPlugin("proximitysensor");
    remoteSensorManager.loadPlugin("rotationsensor");
    remoteSensorManager.loadPlugin("magnetometersensor");

    // Register interfaces (can this be done inside the plugins?
    remoteSensorManager.registerSensorInterface<OrientationSensorChannelInterface>("orientationsensor");
    remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");
    remoteSensorManager.registerSensorInterface<CompassSensorChannelInterface>("compasssensor");
    remoteSensorManager.registerSensorInterface<TapSensorChannelInterface>("tapsensor");
    remoteSensorManager.registerSensorInterface<ALSSensorChannelInterface>("alssensor");
    remoteSensorManager.registerSensorInterface<ProximitySensorChannelInterface>("proximitysensor");
    remoteSensorManager.registerSensorInterface<RotationSensorChannelInterface>("rotationsensor");
    remoteSensorManager.registerSensorInterface<MagnetometerSensorChannelInterface>("magnetometersensor");
}

void ClientApiTest::init()
{
    //qDebug() << "Run before each test";
    //TODO: Verify that sensord has not crashed.
}

void ClientApiTest::cleanup()
{
    //qDebug() << "Run after each test";
    //TODO: Verify that sensord has not crashed.
}

void ClientApiTest::cleanupTestCase()
{
    //qDebug() << "Run after all test cases";
}

void ClientApiTest::testOrientationSensor()
{
    QString sensorName("orientationsensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    OrientationSensorChannelInterface* sensorIfc = OrientationSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another control session
    OrientationSensorChannelInterface* failIfc = OrientationSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(!failIfc, "Got another control session");

    // Get listen session
    const OrientationSensorChannelInterface* listenIfc = OrientationSensorChannelInterface::listenInterface(sensorName);
    QVERIFY2(listenIfc && listenIfc->isValid(), "Failed to get listen session");
    delete listenIfc;

    // Test properties
    sensorIfc->setInterval(100);

    QVERIFY(sensorIfc->orientation() == qvariant_cast<Unsigned>(sensorIfc->property("orientation")));

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());

    delete sensorIfc;
}

void ClientApiTest::testAccelerometerSensor()
{
    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another control session
    AccelerometerSensorChannelInterface* failIfc = AccelerometerSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(!failIfc, "Got another control session");

    // Get listen session
    const AccelerometerSensorChannelInterface* listenIfc = AccelerometerSensorChannelInterface::listenInterface(sensorName);
    QVERIFY2(listenIfc && listenIfc->isValid(), "Failed to get listen session");
    delete listenIfc;

    // Test properties
    sensorIfc->setInterval(100);

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());

    XYZ sample1 = sensorIfc->get();
    XYZ sample2 = qvariant_cast<XYZ>(sensorIfc->property("value"));
    QVERIFY(sample1 == sample2);

    delete sensorIfc;
}

void ClientApiTest::testMagnetometerSensor()
{
    QString sensorName("magnetometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    MagnetometerSensorChannelInterface* sensorIfc = MagnetometerSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another control session
    MagnetometerSensorChannelInterface* failIfc = MagnetometerSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(!failIfc, "Got another control session");

    // Get listen session
    const MagnetometerSensorChannelInterface* listenIfc = MagnetometerSensorChannelInterface::listenInterface(sensorName);
    QVERIFY2(listenIfc && listenIfc->isValid(), "Failed to get listen session");
    delete listenIfc;

    // Test properties
    sensorIfc->setInterval(100);


    // Need simulated data to make sensible test ouf of this.
    MagneticField sample1 = sensorIfc->magneticField();
    MagneticField sample2 = qvariant_cast<MagneticField>(sensorIfc->property("magneticField"));
    // Background process keeps magnetometer on -- values are changing, and subsequent
    // calls are likely to give different values. This makes thus no sense.
    // QVERIFY(sample1 == sample2);

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());

    delete sensorIfc;
}


void ClientApiTest::testCompassSensor()
{
    QString sensorName("compasssensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    CompassSensorChannelInterface* sensorIfc = CompassSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another control session
    CompassSensorChannelInterface* failIfc = CompassSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(!failIfc, "Got another control session");

    // Get listen session
    const CompassSensorChannelInterface* listenIfc = CompassSensorChannelInterface::listenInterface(sensorName);
    QVERIFY2(listenIfc && listenIfc->isValid(), "Failed to get listen session");
    delete listenIfc;

    // Test properties
    sensorIfc->setInterval(100);

    QVERIFY(sensorIfc->get() == qvariant_cast<Compass>(sensorIfc->property("value")));
    QVERIFY(sensorIfc->useDeclination() == sensorIfc->property("usedeclination"));
    QVERIFY(sensorIfc->declinationValue() == sensorIfc->property("declinationvalue"));

    // Check declination on/off
    bool declinationInUse = sensorIfc->useDeclination();
    sensorIfc->setUseDeclination(!declinationInUse);
    QVERIFY(declinationInUse != sensorIfc->useDeclination());
    sensorIfc->setUseDeclination(!declinationInUse);

    // TODO: Compare declination value against value in gconf


    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());

    delete sensorIfc;
}

void ClientApiTest::testTapSensor()
{
    QString sensorName("tapsensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    TapSensorChannelInterface* sensorIfc = TapSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another control session
    TapSensorChannelInterface* failIfc = TapSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(!failIfc, "Got another control session");

    // Get listen session
    const TapSensorChannelInterface* listenIfc = TapSensorChannelInterface::listenInterface(sensorName);
    QVERIFY2(listenIfc && listenIfc->isValid(), "Failed to get listen session");
    delete listenIfc;

    // Test properties
    sensorIfc->setInterval(100);

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test get and set tap type
    TapSensorChannelInterface::TapSelection tapType = sensorIfc->getTapType();
    QCOMPARE(tapType, TapSensorChannelInterface::SingleDouble);
    sensorIfc->setTapType(TapSensorChannelInterface::Double);
    tapType = sensorIfc->getTapType();
    QCOMPARE(tapType, TapSensorChannelInterface::Double);

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());

    delete sensorIfc;
}

void ClientApiTest::testALSSensor()
{
    QString sensorName("alssensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    // TODO: Because context takes control over ALS in sensord::main(),
    //       we can't test this. (applies to all other possible open
    //       sessions too...)
    //ALSSensorChannelInterface* sensorIfc = ALSSensorChannelInterface::controlInterface(sensorName);
    //QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another control session
    ALSSensorChannelInterface* failIfc = ALSSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(!failIfc, "Got another control session");

    // Get listen session
    const ALSSensorChannelInterface* listenIfc = ALSSensorChannelInterface::listenInterface(sensorName);
    QVERIFY2(listenIfc && listenIfc->isValid(), "Failed to get listen session");
    delete listenIfc;

    // Test properties
    //sensorIfc->setInterval(100);

    // test start
    //QDBusReply<void> reply = sensorIfc->start();
    //QVERIFY(reply.isValid());

    // test stop
    //reply = sensorIfc->stop();
    //QVERIFY(reply.isValid());

    //delete sensorIfc;
}

void ClientApiTest::testProximitySensor()
{
    QString sensorName("proximitysensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    ProximitySensorChannelInterface* sensorIfc = ProximitySensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another control session
    ProximitySensorChannelInterface* failIfc = ProximitySensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(!failIfc, "Got another control session");

    // Get listen session
    const ProximitySensorChannelInterface* listenIfc = ProximitySensorChannelInterface::listenInterface(sensorName);
    QVERIFY2(listenIfc && listenIfc->isValid(), "Failed to get listen session");
    delete listenIfc;

    // Test properties
    sensorIfc->setInterval(100);

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());

    delete sensorIfc;
}

void ClientApiTest::testRotationSensor()
{
    QString sensorName("rotationsensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get control session
    RotationSensorChannelInterface* sensorIfc = RotationSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another control session
    RotationSensorChannelInterface* failIfc = RotationSensorChannelInterface::controlInterface(sensorName);
    QVERIFY2(!failIfc, "Got another control session");

    // Get listen session
    const RotationSensorChannelInterface* listenIfc = RotationSensorChannelInterface::listenInterface(sensorName);
    QVERIFY2(listenIfc && listenIfc->isValid(), "Failed to get listen session");
    delete listenIfc;

    // Test properties
    sensorIfc->setInterval(100);

    bool hasz = sensorIfc->hasZ();
    qDebug() << "Z-axis rotation calculations in use:" << hasz;

    // Need simulated data to make sensible test ouf of this.
    XYZ sample1 = sensorIfc->rotation();
    XYZ sample2 = qvariant_cast<XYZ>(sensorIfc->property("rotation"));
    QVERIFY(sample1 == sample2);

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());

    delete sensorIfc;
}


/**
 * Runs two sensors using the same adaptor/chain simultaneously.
 *
 * For the sake of testing, OrientationSensor and AccelerometerSensor are
 * used. AccelerometerChain is shared.
 *
 * @note Sensord hacks starting context framework things might affect this by
 *       adding other sensors that use the same adaptor.
 */
void ClientApiTest::testCommonAdaptorPipeline()
{
    int DELAY = 250;
    OrientationSensorChannelInterface *orientation;
    AccelerometerSensorChannelInterface *accelerometer;

    orientation = OrientationSensorChannelInterface::controlInterface("orientationsensor");
    QVERIFY2(orientation && orientation->isValid(), "Could not get orientation sensor control channel");
    orientation->setInterval(100);

    orientation->start();
    qDebug() << "Orientation sensor started, waiting for" << DELAY << "ms.";
    QTest::qWait(DELAY);

    accelerometer = AccelerometerSensorChannelInterface::controlInterface("accelerometersensor");
    QVERIFY2(accelerometer && accelerometer->isValid(), "Could not get accelerometer sensor control channel");
    accelerometer->setInterval(100);

    accelerometer->start();
    qDebug() << "Accelerometer sensor started, waiting for" << DELAY << "ms.";
    QTest::qWait(DELAY);

    orientation->stop();
    qDebug() << "Orientation sensor stopped, waiting for" << DELAY << "ms.";
    QTest::qWait(DELAY);

    delete orientation;
    qDebug() << "Orientation sensor destroyed, waiting for" << DELAY << "ms.";
    QTest::qWait(DELAY);

    accelerometer->stop();
    qDebug() << "Accelerometer sensor stopped.";

    delete accelerometer;
}

void ClientApiTest::testListenSessionInitiation()
{
    const OrientationSensorChannelInterface *orientation;

    orientation = OrientationSensorChannelInterface::listenInterface("orientationsensor");

    QVERIFY2(orientation && orientation->isValid(), "Could not get orientation sensor listen channel");
}

QTEST_MAIN(ClientApiTest)
