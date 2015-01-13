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

#include "sensormanagerinterface.h"
#include "orientationsensor_i.h"
#include "accelerometersensor_i.h"
#include "compasssensor_i.h"
#include "tapsensor_i.h"
#include "alssensor_i.h"
#include "proximitysensor_i.h"
#include "rotationsensor_i.h"
#include "magnetometersensor_i.h"
#include "gyroscopesensor_i.h"

#include "clientapitest.h"
#include <QSettings>

ClientApiTest::ClientApiTest()
{
    bufferingSensors.append("magnetometersensor");
}

AbstractSensorChannelInterface* ClientApiTest::getSensor(const QString& sensorName)
{
    if (sensorName == "magnetometersensor")
        return MagnetometerSensorChannelInterface::interface(sensorName);
    if (sensorName == "accelerometersensor")
        return AccelerometerSensorChannelInterface::interface(sensorName);
    if (sensorName == "rotationsensor")
        return RotationSensorChannelInterface::interface(sensorName);
    return NULL;
}

void ClientApiTest::calcAverages(const QVector<XYZ>& data, float& x, float& y,  float& z) const
{
    foreach(const XYZ& dataReal, data)
    {
        x += dataReal.x();
        y += dataReal.y();
        z += dataReal.z();
    }
    x /= data.size();
    y /= data.size();
    z /= data.size();
}

void ClientApiTest::calcMaggeAverages(const QVector<MagneticField>& data, float& x, float& y,  float& z, float& rx, float& ry,  float& rz) const
{
    foreach(const MagneticField& dataReal, data)
    {
        x += dataReal.x();
        y += dataReal.y();
        z += dataReal.z();
        rx += dataReal.rx();
        ry += dataReal.ry();
        rz += dataReal.rz();
    }
    x = (float)x / data.size();
    y = (float)y / data.size();
    z = (float)z / data.size();
    rx = (float)rx / data.size();
    ry = (float)ry / data.size();
    rz = (float)rz / data.size();
}

long ClientApiTest::getLimit(AbstractSensorChannelInterface *sensor) const
{
    QList<DataRange> ranges = sensor->getAvailableDataRanges();
    if (ranges.size() > 0)
        return qAbs(ranges.at(0).max - ranges.at(0).min);
    return 1000;
}

void ClientApiTest::initTestCase()
{
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
    remoteSensorManager.loadPlugin("gyroscopesensor");

    // Register interfaces (can this be done inside the plugins?
    remoteSensorManager.registerSensorInterface<OrientationSensorChannelInterface>("orientationsensor");
    remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");
    remoteSensorManager.registerSensorInterface<CompassSensorChannelInterface>("compasssensor");
    remoteSensorManager.registerSensorInterface<TapSensorChannelInterface>("tapsensor");
    remoteSensorManager.registerSensorInterface<ALSSensorChannelInterface>("alssensor");
    remoteSensorManager.registerSensorInterface<ProximitySensorChannelInterface>("proximitysensor");
    remoteSensorManager.registerSensorInterface<RotationSensorChannelInterface>("rotationsensor");
    remoteSensorManager.registerSensorInterface<MagnetometerSensorChannelInterface>("magnetometersensor");
    remoteSensorManager.registerSensorInterface<GyroscopeSensorChannelInterface>("gyroscopesensor");
}

void ClientApiTest::init()
{
}

void ClientApiTest::cleanup()
{
}

void ClientApiTest::cleanupTestCase()
{
}

void ClientApiTest::testOrientationSensor()
{
    QString sensorName("orientationsensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    OrientationSensorChannelInterface* sensorIfc = OrientationSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensorIfc);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");


    // Attempt to get another session
    OrientationSensorChannelInterface* sensorIfc2 = OrientationSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(sensorIfc2);
    QVERIFY2(sensorIfc2, "Failed to get another session");

    // Test properties
    sensorIfc->setInterval(100);

    QVERIFY(sensorIfc->orientation() == qvariant_cast<Unsigned>(sensorIfc->property("orientation")));

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());

}

void ClientApiTest::testAccelerometerSensor()
{
    QString sensorName("accelerometersensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensorIfc);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another session
    AccelerometerSensorChannelInterface* sensorIfc2 = AccelerometerSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(sensorIfc2);
    QVERIFY2(sensorIfc2, "Failed to get another session");

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

}

void ClientApiTest::testGyroscopeSensor()
{
    QString sensorName("gyroscopesensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    GyroscopeSensorChannelInterface* sensorIfc = GyroscopeSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another session
    GyroscopeSensorChannelInterface* sensorIfc2 = GyroscopeSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc2, "Failed to get another session");
    delete sensorIfc2;

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

    // Get session
    MagnetometerSensorChannelInterface* sensorIfc = MagnetometerSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensorIfc);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    MagnetometerSensorChannelInterface* sensorIfc2 = MagnetometerSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(sensorIfc2);
    QVERIFY2(sensorIfc2, "Failed to get another session");

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

}

void ClientApiTest::testCompassSensor()
{
    QString sensorName("compasssensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    CompassSensorChannelInterface* sensorIfc = CompassSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensorIfc);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    CompassSensorChannelInterface* sensorIfc2 = CompassSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(sensorIfc2);
    QVERIFY2(sensorIfc2, "Failed to get another session");

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

    QSettings confFile("/etc/xdg/sensorfw/location.conf", QSettings::IniFormat);
    confFile.beginGroup("location");
    confFile.setValue("declination",50);

    sensorIfc->setUseDeclination(true);
    QVERIFY(sensorIfc->declinationValue() == 50);
    sensorIfc2->setUseDeclination(false);

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());
    reply = sensorIfc2->start();
    QVERIFY(reply.isValid());

    QTest::qWait(2000);

    Compass value1(sensorIfc->get());
    Compass value2(sensorIfc2->get());

    QVERIFY(value1.degrees() != value2.degrees());
    QVERIFY((value2.degrees() + 50 % 360) == value1.degrees());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());
    reply = sensorIfc2->stop();
    QVERIFY(reply.isValid());

   confFile.setValue("declination",0);
}

void ClientApiTest::testTapSensor()
{
    QString sensorName("tapsensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    TapSensorChannelInterface* sensorIfc = TapSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensorIfc);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    TapSensorChannelInterface* sensorIfc2 = TapSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(sensorIfc2);
    QVERIFY2(sensorIfc2, "Failed to get another session");

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
}

void ClientApiTest::testALSSensor()
{
    QString sensorName("alssensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    // TODO: Because context takes control over ALS in sensord::main(),
    //       we can't test this. (applies to all other possible open
    //       sessions too...)

    ALSSensorChannelInterface* sensorIfc = ALSSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensorIfc);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Get another session
    ALSSensorChannelInterface* sensorIfc2 = ALSSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(sensorIfc2);
    QVERIFY2(sensorIfc2, "Failed to get another session");

    // Test properties
    sensorIfc->setInterval(100);

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());
}

void ClientApiTest::testProximitySensor()
{
    QString sensorName("proximitysensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    ProximitySensorChannelInterface* sensorIfc = ProximitySensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensorIfc);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    ProximitySensorChannelInterface* sensorIfc2 = ProximitySensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(sensorIfc2);
    QVERIFY2(sensorIfc2, "Failed to get another session");

    // Test properties
    sensorIfc->setInterval(100);

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());
}

void ClientApiTest::testRotationSensor()
{
    QString sensorName("rotationsensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    RotationSensorChannelInterface* sensorIfc = RotationSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensorIfc);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    RotationSensorChannelInterface* sensorIfc2 = RotationSensorChannelInterface::interface(sensorName);
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(sensorIfc2);
    QVERIFY2(sensorIfc2, "Failed to get another session");


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

    orientation = OrientationSensorChannelInterface::interface("orientationsensor");
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp(orientation);
    QVERIFY2(orientation && orientation->isValid(), "Could not get orientation sensor channel");
    orientation->setInterval(100);

    orientation->start();
    qDebug() << "Orientation sensor started, waiting for" << DELAY << "ms.";
    QTest::qWait(DELAY);

    accelerometer = AccelerometerSensorChannelInterface::interface("accelerometersensor");
    QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(accelerometer);
    QVERIFY2(accelerometer && accelerometer->isValid(), "Could not get accelerometer sensor channel");
    accelerometer->setInterval(100);

    accelerometer->start();
    qDebug() << "Accelerometer sensor started, waiting for" << DELAY << "ms.";
    QTest::qWait(DELAY);

    orientation->stop();
    qDebug() << "Orientation sensor stopped, waiting for" << DELAY << "ms.";
    QTest::qWait(DELAY);

    qDebug() << "Orientation sensor destroyed, waiting for" << DELAY << "ms.";
    QTest::qWait(DELAY);

    accelerometer->stop();
    qDebug() << "Accelerometer sensor stopped.";
}

void ClientApiTest::testSessionInitiation()
{
    const OrientationSensorChannelInterface *orientation;

    orientation = OrientationSensorChannelInterface::interface("orientationsensor");
    QScopedPointer<const OrientationSensorChannelInterface> sensorTmp(orientation);
    QVERIFY2(orientation && orientation->isValid(), "Could not get orientation sensor channel");
}

void ClientApiTest::testBuffering()
{
    foreach(const QString& sensorName, bufferingSensors)
    {
        AbstractSensorChannelInterface* sensor = getSensor(sensorName);
        QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensor);
        QVERIFY2(sensor && sensor->isValid(),QString("Could not get %1 sensor channel").arg(sensorName).toLatin1());
        TestClient client(*sensor, true);
        int bufferSize = 10;
        int interval = 100;
        sensor->setInterval(interval);
        sensor->setBufferSize(bufferSize);
        sensor->setBufferInterval(bufferSize * interval * 1.5);
        sensor->setDownsampling(true);
        sensor->setStandbyOverride(true);

        sensor->start();
        client.resetTimers();

        int dataLimit = 4;
        int frameLimit = 1;

        int period = bufferSize * interval * 1.2;   // within [bufferSize * interval, bufferingInterval]

        for (int i = 0; i < 2; i++)
        {
            qDebug() << sensorName << " started, waiting for " << period << " ms.";
            QTest::qWait(period);
            int dataCount = client.getDataCount();

            //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
            QVERIFY2( dataCount < dataLimit * (i + 1), errorMessage(sensorName, interval, dataCount, "<", dataLimit * (i + 1)));
            int frameCount = client.getFrameCount();
            QVERIFY2( frameCount == frameLimit * (i + 1), errorMessage(sensorName, interval, frameCount, "==", frameLimit * (i + 1)));
            int frameDataCount = client.getFrameDataCount();
            QVERIFY2(frameDataCount == bufferSize * (i + 1), errorMessage(sensorName, interval, frameDataCount, "==", bufferSize * (i + 1)));
            client.resetCounters();
            period += (bufferSize * interval);
        }
        sensor->stop();
    }
}

void ClientApiTest::testBufferingAllIntervalRanges()
{
    foreach(const QString& sensorName, bufferingSensors)
    {
        AbstractSensorChannelInterface* sensor = getSensor(sensorName);
        QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensor);
        QVERIFY2(sensor && sensor->isValid(),QString("Could not get %1 sensor channel").arg(sensorName).toLatin1());
        TestClient client(*sensor, true);

        int bufferSize = 100;
        int dataLimit = 2;
        int frameLimit = 1;

        foreach(const DataRange& range, sensor->getAvailableIntervals())
        {
            qreal intervalMax = range.max;
            qreal intervalMin = range.min;

            if (intervalMin == 0 && intervalMax == 0)
                continue;
            if (intervalMax >= 100)
                continue; //let's stop to 10Hz
            QList<qreal> intervalTests;
            if (intervalMin != 0)
                intervalTests.append(intervalMin);
            if (intervalMin != intervalMax) {
                intervalTests.append(intervalMax);
                intervalTests.append((intervalMin + intervalMax) * 0.5);
            }
            foreach(qreal interval, intervalTests)
            {
                sensor->setInterval(interval);
                sensor->setBufferSize(bufferSize);
                sensor->setBufferInterval(interval * bufferSize * 3);
                sensor->setDownsampling(false);
                sensor->setStandbyOverride(true);
                sensor->start();
                client.resetTimers();

                int period = interval * bufferSize * 1.25;
                qDebug() << sensorName << " started, waiting for " << period << " ms.";
                QTest::qWait(period);
                int dataCount = client.getDataCount();
                //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
                QVERIFY2( dataCount<dataLimit, errorMessage(sensorName, interval, dataCount, "<", dataLimit));

                int frameCount = client.getFrameCount();
                QVERIFY2( frameCount == frameLimit, errorMessage(sensorName, interval, frameCount, "==", frameLimit));

                int frameDataCount = client.getFrameDataCount();
                QVERIFY2( frameDataCount == bufferSize, errorMessage(sensorName, interval, frameDataCount, "==", bufferSize));
                sensor->stop();
                client.resetCounters();
            }
        }
    }
}

void ClientApiTest::testBufferingCompatibility()
{
    foreach(const QString& sensorName, bufferingSensors)
    {
        AbstractSensorChannelInterface* sensor = getSensor(sensorName);
        QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensor);
        QVERIFY2(sensor && sensor->isValid(),QString("Could not get %1 sensor channel").arg(sensorName).toLatin1());
        TestClient client(*sensor, false);
        int interval = 100;
        int bufferSize = 10;
        sensor->setInterval(interval);
        sensor->setBufferSize(bufferSize);
        sensor->setBufferInterval(2 * interval * bufferSize);
        sensor->setDownsampling(false);
        sensor->setStandbyOverride(true);
        sensor->start();
        client.resetTimers();

        int period = 1.5 * interval * bufferSize;
        qDebug() << sensorName<<" started, waiting for "<<period<<" ms.";
        QTest::qWait(period);

        //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
        int dataCount = client.getDataCount();
        QVERIFY2( dataCount == bufferSize, errorMessage(sensorName, interval, dataCount, "==", bufferSize));

        int frameCount = client.getFrameCount();
        QVERIFY2( frameCount == 0, errorMessage(sensorName, interval, frameCount, "==",0));
        client.resetCounters();

        period  =  interval * bufferSize * 1.1;
        qDebug() << sensorName << " started, waiting for " << period << " ms.";
        QTest::qWait(period);

        //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
        dataCount = client.getDataCount();
        QVERIFY2( dataCount==bufferSize, errorMessage(sensorName, interval, dataCount, "==", bufferSize));

        frameCount = client.getFrameCount();
        QVERIFY2( frameCount == 0, errorMessage(sensorName, interval, frameCount, "==",0));
        QVERIFY(client.getFrameDataCount() == 0);

        sensor->stop();
    }
}

void ClientApiTest::testBufferingInterval()
{
    foreach(const QString& sensorName, bufferingSensors)
    {
        AbstractSensorChannelInterface* sensor = getSensor(sensorName);
        QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensor);
        QVERIFY2(sensor && sensor->isValid(),QString("Could not get %1 sensor channel").arg(sensorName).toLatin1());
        TestClient client(*sensor, true);
        int interval = 100;
        sensor->setInterval(interval);
        int bufferInterval = 0;
        sensor->setBufferInterval(bufferInterval);
        int bufferSize = 40;
        sensor->setBufferSize(bufferSize);
        sensor->setDownsampling(true);
        sensor->setStandbyOverride(true);

        sensor->start();
        client.resetTimers();

        int period = interval * bufferSize * 1.2;
        qDebug() << sensorName << " started, waiting for " << period << " ms.";
        QTest::qWait(period);

        int dataCount = client.getDataCount();
        //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
        int limit = 4;
        QVERIFY2( dataCount < limit, errorMessage(sensorName, interval, dataCount, "<", limit));
        int frameCount =client.getFrameCount();
        limit = period / (bufferSize * interval);
        QVERIFY2( frameCount == limit, errorMessage(sensorName, interval, frameCount, "==", limit));
        int frameDataCount = client.getFrameDataCount();
        QVERIFY2( frameDataCount == bufferSize, errorMessage(sensorName, interval, frameDataCount, "==", bufferSize));

        sensor->stop();
        client.resetCounters();

        bufferInterval = interval * bufferSize * 0.5;
        sensor->setBufferInterval(bufferInterval);
        sensor->setBufferSize(bufferSize);
        sensor->setStandbyOverride(true);

        sensor->start();
        client.resetTimers();

        period = interval * bufferSize * 0.7;

        // one more frame should be coming
        qDebug() << sensorName<<" started, waiting for "<<period<<" ms.";
        QTest::qWait(period);
        dataCount = client.getDataCount();

        //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
        limit = 4;
        QVERIFY2( dataCount < limit, errorMessage(sensorName, interval, dataCount, "<", limit));
        frameCount = client.getFrameCount();
        QVERIFY2( frameCount == 1, errorMessage(sensorName, interval, frameCount, "==", 1));

        frameDataCount = client.getFrameDataCount();
        QVERIFY2( frameDataCount < bufferSize, errorMessage(sensorName, interval, frameDataCount, "<=", bufferSize));

        sensor->stop();
    }
}

void ClientApiTest::testAvailableBufferIntervals()
{
    foreach(const QString& sensorName, bufferingSensors)
    {
        AbstractSensorChannelInterface* sensor = getSensor(sensorName);
        QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensor);
        QVERIFY2(sensor && sensor->isValid(),QString("Could not get %1 sensor channel").arg(sensorName).toLatin1());

        IntegerRangeList rangeList = sensor->getAvailableBufferIntervals();
        QVERIFY(rangeList.size() == 1);
        QVERIFY(rangeList.front().first == 0);
        QVERIFY(rangeList.front().second == 60000); //default max value for buffer if no hw buffering, see nodebase.cpp

        sensor->stop();
    }
}

void ClientApiTest::testAvailableBufferSizes()
{
    foreach(const QString& sensorName, bufferingSensors)
    {
        AbstractSensorChannelInterface* sensor = getSensor(sensorName);
        QScopedPointer<AbstractSensorChannelInterface> sensorTmp(sensor);
        QVERIFY2(sensor && sensor->isValid(),QString("Could not get %1 sensor channel").arg(sensorName).toLatin1());
        IntegerRangeList rangeList = sensor->getAvailableBufferSizes();
        QVERIFY(rangeList.size() == 1);
        QVERIFY(rangeList.front().first == 1);
        QVERIFY(rangeList.front().second == 256);

        sensor->stop();
    }
}

void ClientApiTest::testDownsampling()
{
    foreach(const QString& sensorName, bufferingSensors)
    {
        AbstractSensorChannelInterface* sensor1 = getSensor(sensorName);
        AbstractSensorChannelInterface* sensor2 = getSensor(sensorName);
        QScopedPointer<AbstractSensorChannelInterface> sensorTmp1(sensor1);
        QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(sensor2);
        QVERIFY2(sensor1 && sensor1->isValid(),QString("Could not get %1 sensor channel").arg(sensorName).toLatin1());
        QVERIFY2(sensor2 && sensor2->isValid(),QString("Could not get %1 sensor channel").arg(sensorName).toLatin1());

        SampleCollector client1(*sensor1, true);
        int interval = 100;
        sensor1->setInterval(interval);
        sensor1->setStandbyOverride(true);

        sensor1->start();
        client1.resetTimers();

        SampleCollector client2(*sensor2, true);
        int interval2 = 10*interval;
        sensor2->setInterval(interval2);
        sensor2->setDownsampling(true);
        sensor2->setStandbyOverride(true);

        sensor2->start();
        client2.resetTimers();

        int period =interval2 * 2 + 500;
        qDebug() << sensorName << " started, waiting for " << period << " ms.";
        QTest::qWait(period);

        sensor1->stop();
        sensor2->stop();

        int limit = period / interval * 0.9;    //90% of calculated size
        int sampleCount = sensorName == "magnetometersensor" ? client1.getSamples1().size() : client1.getSamples2().size();
        QVERIFY2( sampleCount >= limit, errorMessage(sensorName, interval, sampleCount, ">=", limit));

        limit = period / interval2 ;
        sampleCount = sensorName == "magnetometersensor" ? client2.getSamples1().size() : client2.getSamples2().size();
        QVERIFY2( sampleCount == limit, errorMessage(sensorName, interval, sampleCount, "==", limit));

        float x1 = 0, y1 = 0, z1 = 0, rx1 = 0, ry1 = 0, rz1 = 0;
        float x2 = 0, y2 = 0, z2 = 0, rx2 = 0, ry2 = 0, rz2 = 0;

        if (sensorName != "magnetometersensor")
            calcAverages(client1.getSamples2(), x1, y1, z1);
        else
            calcMaggeAverages(client1.getSamples1(), x1, y1, z1, rx1, ry1, rz1);

//        qDebug()<<"calcMaggeAverages - after calculation:  x1 = "<<x1 <<" y1="<<y1<<" z1="<<z1<< " rx1="<<rx1<<" ry1="<<ry1<<" rz1="<<rz1;

        if (sensorName != "magnetometersensor")
            calcAverages(client2.getSamples2(), x2, y2, z2);
        else
            calcMaggeAverages(client2.getSamples1(), x2, y2, z2, rx2, ry2, rz2);

//        qDebug()<<"calcMaggeAverages - after calculation:  x2 = "<<x2 <<" y2="<<y2<<" z2="<<z2<< " rx2="<<rx2<<" ry2="<<ry2<<" rz2="<<rz2;

        long rangeLimit = getLimit(sensor1);
//        qDebug()<<sensorName<<"range = "<<rangeLimit<<" x1="<<x1<<" y1="<<y1<<" z1="<<z1<<" x2="<<x2<<" y2="<<y2<<" z2="<<z2<<" rx1="<<rx1<<" ry1="<<ry1<<" rz1="<<rz1<<" rx2="<<rx2<<" ry2="<<ry2<<" rz2="<<rz2;

        //since instances were not started at the same time there may be few samples of difference...
        // error less than 30% of total range is accepted
        float limitF = 0.3f;

        QVERIFY2((float)abs(x1 - x2) / rangeLimit < limitF, errorMessage(sensorName, interval, (float)abs(x1 - x2) / rangeLimit, "<", limitF ));
        QVERIFY2((float)abs(y1 - y2) / rangeLimit < limitF, errorMessage(sensorName, interval, (float)abs(y1 - y2) / rangeLimit,"<", limitF ));
        QVERIFY2((float)abs(rx1 - rx2) / rangeLimit < limitF, errorMessage(sensorName, interval, (float)abs(rx1 - rx2) / rangeLimit,"<", limitF ));
        QVERIFY2((float)abs(ry1 - ry2) / rangeLimit < limitF, errorMessage(sensorName, interval, (float)abs(ry1 - ry2) / rangeLimit,"<", limitF ));
    }
}

void ClientApiTest::testDownsamplingDisabled()
{
    foreach(const QString& sensorName, bufferingSensors)
    {
        AbstractSensorChannelInterface* sensor1 = getSensor(sensorName);
        AbstractSensorChannelInterface* sensor2 = getSensor(sensorName);
        QScopedPointer<AbstractSensorChannelInterface> sensorTmp1(sensor1);
        QScopedPointer<AbstractSensorChannelInterface> sensorTmp2(sensor2);

        QVERIFY2(sensor1 && sensor1->isValid(),QString("Could not get %1 sensor channel").arg(sensorName).toLatin1());
        QVERIFY2(sensor2 && sensor2->isValid(),QString("Could not get %1 sensor channel").arg(sensorName).toLatin1());

        SampleCollector client1(*sensor1, true);
        int interval = 100;
        sensor1->setInterval(interval);
        sensor1->setStandbyOverride(true);
        sensor1->start();

        SampleCollector client2(*sensor2, true);
        int interval2 = 10 * interval;
        sensor2->setInterval(interval2);
        sensor2->setDownsampling(false);
        sensor2->setStandbyOverride(true);
        sensor2->start();

        // waiting for two frames to come
        int period = qMax(interval, interval2) * 2.2;
        qDebug() << sensorName <<" started, waiting for "<<period<<" ms.";
        QTest::qWait(period);

        sensor1->stop();
        sensor2->stop();

        // sensor
        int dataCount = sensorName == "magnetometersensor" ? client1.getSamples1().size() : client1.getSamples2().size();
        int limit = period / interval * 0.9;
        QVERIFY2( dataCount >= limit, errorMessage(sensorName, interval, dataCount, ">=", limit));

        // when no downsampling the two sensors should get the about the same amount of samples
        int dataCount2 = sensorName == "magnetometersensor"?client2.getSamples1().size():client2.getSamples2().size();

        int sampleCountDiff = abs(dataCount - dataCount2);
        limit = 2;  //for leaks
        QVERIFY2( sampleCountDiff < limit, errorMessage(sensorName, interval, sampleCountDiff, "<", limit));
    }
}

TestClient::TestClient(AbstractSensorChannelInterface& iface, bool listenFrames) :
    dataCount(0),
    frameCount(0),
    frameDataCount(0)
{
    QString id = iface.id();
    if (id == "magnetometersensor"){
        connect(&iface, SIGNAL(dataAvailable(const MagneticField&)), this, SLOT(dataAvailable(const MagneticField&)));
        if(listenFrames)
            connect(&iface, SIGNAL(frameAvailable(const QVector<MagneticField>&)), this, SLOT(frameAvailable(const QVector<MagneticField>&)));
        return;
    }

    connect(&iface, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(dataAvailable2(const XYZ&)));
    if(listenFrames)
        connect(&iface, SIGNAL(frameAvailable(const QVector<XYZ>&)), this, SLOT(frameAvailable2(const QVector<XYZ>&)));

}

void TestClient::resetTimers(){
    m_exTimeData = QTime::currentTime();
    m_exTimeFrame = QTime::currentTime();
}

void TestClient::resetCounters(){
    dataCount = 0;
    frameCount = 0;
    frameDataCount = 0;
}

TestClient::~TestClient()
{
}

void TestClient::dataAvailable(const MagneticField&)
{
    QTime now = QTime::currentTime();
    qDebug() << "dataAvailable() "
             << ++dataCount << " in "
             << (dataCount > -1 ? m_exTimeData.msecsTo(now) : 0)
             << " ms" ;
    m_exTimeData = now;
}

void TestClient::frameAvailable(const QVector<MagneticField>& frame)
{
    QTime now = QTime::currentTime();
    qDebug() << "frameAvailable(): "
             << frame.size()
             << " in "
             << (frameCount >-1 ? m_exTimeFrame.msecsTo(now) : 0)
             << " ms";
    m_exTimeFrame = now;
    ++frameCount;
    frameDataCount += frame.size();
}

void TestClient::dataAvailable2(const XYZ&)
{
    QTime now = QTime::currentTime();
    qDebug() << "dataAvailable() "
             << ++dataCount
             << " in "
             << (dataCount > -1 ? m_exTimeData.msecsTo(now) : 0)
             << " ms";
    m_exTimeData = now;
}

void TestClient::frameAvailable2(const QVector<XYZ>& frame)
{
    QTime now = QTime::currentTime();
    qDebug() << "frameAvailable(): "
             << frame.size()
             << " in "
             << (frameCount > -1 ? m_exTimeFrame.msecsTo(now) : 0)
             << " ms";
    m_exTimeFrame = now;
    ++frameCount;
    frameDataCount += frame.size();
}

SampleCollector::SampleCollector(AbstractSensorChannelInterface& iface, bool listenFrames) :
        TestClient(iface, listenFrames)
{
}

SampleCollector::~SampleCollector()
{
}

void SampleCollector::dataAvailable(const MagneticField& data)
{
    samples1.push_back(data);
    TestClient::dataAvailable(data);
}

void SampleCollector::frameAvailable(const QVector<MagneticField>& frame)
{
    foreach(const MagneticField& data, frame)
    {
        samples1.push_back(data);
    }
    TestClient::frameAvailable(frame);
}

void SampleCollector::dataAvailable2(const XYZ& data)
{
    samples2.push_back(data);
    TestClient::dataAvailable2(data);
}

void SampleCollector::frameAvailable2(const QVector<XYZ>& frame)
{
    foreach(const XYZ& data, frame)
    {
        samples2.push_back(data);
    }
    TestClient::frameAvailable2(frame);
}

QTEST_MAIN(ClientApiTest)
