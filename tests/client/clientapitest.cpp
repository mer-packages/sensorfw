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

void ClientApiTest::calcAverages(QVector<QObject*> data, long& x, long& y,  long& z){
    int l = data.size();
    for (int i=0; i<l; i++){
        XYZ* dataReal = (XYZ*)(data.at(i));
        x += dataReal->x();
        y += dataReal->y();
        z += dataReal->z();
    }
    x/=l;
    y/=l;
    z/=1;
}


void ClientApiTest::calcMaggeAverages(QVector<QObject*> data, long& x, long& y,  long& z, long& rx, long& ry,  long& rz){
    int l = data.size();
    for (int i=0; i<l; i++){
        MagneticField* dataReal = (MagneticField*)(data.at(i));
        x += dataReal->x();
        y += dataReal->y();
        z += dataReal->z();
        rx += dataReal->rx();
        ry += dataReal->ry();
        rz += dataReal->rz();
    }
    x/=l;
    y/=l;
    z/=1;
    rx/=l;
    ry/=l;
    rz/=1;
}

long ClientApiTest::getLimit(AbstractSensorChannelInterface *sensor){
    QList<DataRange> ranges = sensor->getAvailableDataRanges();
    if (ranges.size()>0)
        return  qAbs(ranges.at(0).max - ranges.at(0).min);
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
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    OrientationSensorChannelInterface* sensorIfc2 = OrientationSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc2, "Failed to get another session");
    delete sensorIfc2;

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

    // Get session
    AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get control session");

    // Attempt to get another session
    AccelerometerSensorChannelInterface* sensorIfc2 = AccelerometerSensorChannelInterface::interface(sensorName);
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
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    MagnetometerSensorChannelInterface* sensorIfc2 = MagnetometerSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc2, "Failed to get another session");
    delete sensorIfc2;

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

    // Get session
    CompassSensorChannelInterface* sensorIfc = CompassSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    CompassSensorChannelInterface* sensorIfc2 = CompassSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc2, "Failed to get another session");
    delete sensorIfc2;

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

    // Get session
    TapSensorChannelInterface* sensorIfc = TapSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    TapSensorChannelInterface* sensorIfc2 = TapSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc2, "Failed to get another session");
    delete sensorIfc2;

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

    // Get session
    // TODO: Because context takes control over ALS in sensord::main(),
    //       we can't test this. (applies to all other possible open
    //       sessions too...)

    ALSSensorChannelInterface* sensorIfc = ALSSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Get another session
    ALSSensorChannelInterface* sensorIfc2 = ALSSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc2, "Failed to get another session");
    delete sensorIfc2;

    // Test properties
    //sensorIfc->setInterval(100);

    // test start
    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY(reply.isValid());

    // test stop
    reply = sensorIfc->stop();
    QVERIFY(reply.isValid());

    delete sensorIfc;
}

void ClientApiTest::testProximitySensor()
{
    QString sensorName("proximitysensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    ProximitySensorChannelInterface* sensorIfc = ProximitySensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    ProximitySensorChannelInterface* sensorIfc2 = ProximitySensorChannelInterface::interface(sensorName);
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

    delete sensorIfc;
}

void ClientApiTest::testRotationSensor()
{
    QString sensorName("rotationsensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    // Get session
    RotationSensorChannelInterface* sensorIfc = RotationSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get session");

    // Attempt to get another session
    RotationSensorChannelInterface* sensorIfc2 = RotationSensorChannelInterface::interface(sensorName);
    QVERIFY2(sensorIfc2, "Failed to get another session");
    delete sensorIfc2;

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

    orientation = OrientationSensorChannelInterface::interface("orientationsensor");
    QVERIFY2(orientation && orientation->isValid(), "Could not get orientation sensor channel");
    orientation->setInterval(100);

    orientation->start();
    qDebug() << "Orientation sensor started, waiting for" << DELAY << "ms.";
    QTest::qWait(DELAY);

    accelerometer = AccelerometerSensorChannelInterface::interface("accelerometersensor");
    QVERIFY2(accelerometer && accelerometer->isValid(), "Could not get accelerometer sensor channel");
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

void ClientApiTest::testSessionInitiation()
{
    const OrientationSensorChannelInterface *orientation;

    orientation = OrientationSensorChannelInterface::interface("orientationsensor");
    QVERIFY2(orientation && orientation->isValid(), "Could not get orientation sensor channel");
}

void ClientApiTest::testBuffering()
{
    MagnetometerSensorChannelInterface* magnetometer = MagnetometerSensorChannelInterface::interface("magnetometersensor");
    QVERIFY2(magnetometer && magnetometer->isValid(), "Could not get magnetometer sensor channel");
    TestClient client(*magnetometer, true);
    magnetometer->setInterval(100);
    magnetometer->setBufferSize(10);
    magnetometer->setBufferInterval(1500);
    magnetometer->setDownsampling(false);

    magnetometer->start();

    qDebug() << "Magnetometer sensor started, waiting for 1400ms.";
    QTest::qWait(1400);

    QVERIFY(client.getDataCount() < 2); //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
    QVERIFY(client.getFrameCount() == 1);
    QVERIFY(client.getFrameDataCount() == 10);

    qDebug() << "Magnetometer sensor started, waiting for 1400ms.";
    QTest::qWait(1200);

    QVERIFY(client.getDataCount() < 2); //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
    QVERIFY(client.getFrameCount() == 2);
    QVERIFY(client.getFrameDataCount() == 20);

    magnetometer->stop();
    delete magnetometer;
}

void ClientApiTest::testBufferingHighRate()
{
    MagnetometerSensorChannelInterface* magnetometer = MagnetometerSensorChannelInterface::interface("magnetometersensor");
    QVERIFY2(magnetometer && magnetometer->isValid(), "Could not get magnetometer sensor channel");
    TestClient client(*magnetometer, true);
    magnetometer->setInterval(25);
    magnetometer->setBufferSize(100);
    magnetometer->setBufferInterval(6000);
    magnetometer->setDownsampling(false);

    magnetometer->start();

    qDebug() << "Magnetometer sensor started, waiting for 3500ms.";
    QTest::qWait(3500);

    QVERIFY(client.getDataCount() < 2); //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
    QVERIFY(client.getFrameCount() == 1);
    QVERIFY(client.getFrameDataCount() == 100);

    magnetometer->stop();
    delete magnetometer;
}

void ClientApiTest::testBufferingCompatibility()
{
    MagnetometerSensorChannelInterface* magnetometer = MagnetometerSensorChannelInterface::interface("magnetometersensor");
    QVERIFY2(magnetometer && magnetometer->isValid(), "Could not get magnetometer sensor channel");
    TestClient client(*magnetometer, false);
    magnetometer->setInterval(100);
    magnetometer->setBufferSize(10);
    magnetometer->setBufferInterval(1500);
    magnetometer->setDownsampling(false);

    magnetometer->start();

    qDebug() << "Magnetometer sensor started, waiting for 1400ms.";
    QTest::qWait(1200);

    QVERIFY(client.getDataCount() == 10);
    QVERIFY(client.getFrameCount() == 0);
    QVERIFY(client.getFrameDataCount() == 0);

    qDebug() << "Magnetometer sensor started, waiting for 1400ms.";
    QTest::qWait(1200);

    QVERIFY(client.getDataCount() == 20);
    QVERIFY(client.getFrameCount() == 0);
    QVERIFY(client.getFrameDataCount() == 0);

    magnetometer->stop();
    delete magnetometer;
}

void ClientApiTest::testBufferingInterval()
{
    MagnetometerSensorChannelInterface* magnetometer = MagnetometerSensorChannelInterface::interface("magnetometersensor");
    QVERIFY2(magnetometer && magnetometer->isValid(), "Could not get magnetometer sensor channel");
    TestClient client(*magnetometer, true);
    magnetometer->setInterval(100);
    magnetometer->setBufferInterval(0);
    magnetometer->setBufferSize(40);
    magnetometer->setDownsampling(false);

    magnetometer->start();

    qDebug() << "Magnetometer sensor started, waiting for 4500ms.";
    QTest::qWait(4500);

    QVERIFY(client.getDataCount() < 4); //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
    QVERIFY(client.getFrameCount() == 1);
    QVERIFY(client.getFrameDataCount() == 40);

    magnetometer->stop();
    magnetometer->setBufferInterval(2000);
    magnetometer->setBufferSize(40);
    magnetometer->start();

    qDebug() << "Magnetometer sensor started, waiting for 2500ms.";
    QTest::qWait(2500);

    QVERIFY(client.getDataCount() < 8); //NOTE: because how sensors are configured (sensor started then configured) it is possible that few values can leak.
    QVERIFY(client.getFrameCount() == 2);
    QVERIFY(client.getFrameDataCount() < 80);

    magnetometer->stop();
    delete magnetometer;
}

void ClientApiTest::testAvailableBufferIntervals()
{
    MagnetometerSensorChannelInterface* magnetometer = MagnetometerSensorChannelInterface::interface("magnetometersensor");
    QVERIFY2(magnetometer && magnetometer->isValid(), "Could not get magnetometer sensor channel");
    IntegerRangeList rangeList = magnetometer->getAvailableBufferIntervals();
    QVERIFY(rangeList.size() == 1);
    QVERIFY(rangeList.front().first == 0);
    QVERIFY(rangeList.front().second == 60000);

    magnetometer->stop();
    delete magnetometer;
}

void ClientApiTest::testAvailableBufferSizes()
{
    MagnetometerSensorChannelInterface* magnetometer = MagnetometerSensorChannelInterface::interface("magnetometersensor");
    QVERIFY2(magnetometer && magnetometer->isValid(), "Could not get magnetometer sensor channel");
    IntegerRangeList rangeList = magnetometer->getAvailableBufferSizes();
    QVERIFY(rangeList.size() == 1);
    QVERIFY(rangeList.front().first == 1);
    QVERIFY(rangeList.front().second == 256);

    magnetometer->stop();
    delete magnetometer;
}

void ClientApiTest::testDownsampling()
{

    QStringList sensorList;
    sensorList.append("magnetometersensor");
    sensorList.append("accelerometersensor");
    sensorList.append("rotationsensor");

    for (int i=0, l=sensorList.size(); i<l; i++){
        QString sensorName = sensorList.at(i);

        AbstractSensorChannelInterface* sensor;
        AbstractSensorChannelInterface* sensor2;
        if (sensorName == "magnetometersensor"){
            sensor = MagnetometerSensorChannelInterface::interface(sensorName);
            sensor2 = MagnetometerSensorChannelInterface::interface(sensorName);
        }
        else if (sensorName == "accelerometersensor"){
            sensor = AccelerometerSensorChannelInterface::interface(sensorName);
            sensor2 = AccelerometerSensorChannelInterface::interface(sensorName);
        }
        else if (sensorName == "rotationsensor"){
            sensor = RotationSensorChannelInterface::interface(sensorName);
            sensor2 = RotationSensorChannelInterface::interface(sensorName);
        }
        QString errorMsg("Could not get");
        errorMsg.append(sensorName);
        errorMsg.append(" sensor channel");
        QVERIFY2(sensor && sensor->isValid(),errorMsg.toAscii());
        SampleCollector client1(*sensor, true);
        sensor->setInterval(100);
        sensor->start();

        QVERIFY2(sensor2 && sensor2->isValid(), errorMsg.toAscii());
        SampleCollector client2(*sensor2, true);
        sensor2->setInterval(1000);
        sensor2->setDownsampling(true);
        sensor2->start();

        qDebug() << sensorName<<" started, waiting for 2500ms.";
        QTest::qWait(2500);

        sensor->stop();
        sensor2->stop();
        QVERIFY(client1.getSamples().size() > 20);
        QVERIFY(client2.getSamples().size() == 2);

        long x1 = 0;
        long y1 = 0;
        long z1 = 0;
        long rx1 = 0;
        long ry1 = 0;
        long rz1 = 0;
        long x2 = 0;
        long y2 = 0;
        long z2 = 0;
        long rx2 = 0;
        long ry2 = 0;
        long rz2 = 0;

        if (sensorName!="magnetometersensor") calcAverages(client1.getSamples(), x1, y1, z1);
        else calcMaggeAverages(client1.getSamples(), x1, y1, z1, rx1, ry1, rz1);


        if (sensorName!="magnetometersensor") calcAverages(client2.getSamples(), x2, y2, z2);
        else calcMaggeAverages(client1.getSamples(), x2, y2, z2, rx2, ry2, rz2);

        long limit = getLimit(sensor);

        //since instances were not started at the same time there may be few samples of difference...
        QVERIFY(abs(x1 - x2)/limit < 0.1);
        QVERIFY(abs(y1 - y2)/limit < 0.1);
        QVERIFY(abs(z1 - z2)/limit < 0.1);
        QVERIFY(abs(rx1 - rx2)/limit < 0.1);
        QVERIFY(abs(ry1 - ry2)/limit < 0.1);
        QVERIFY(abs(rz1 - rz2)/limit < 0.1);
        delete sensor;
        delete sensor2;
    }
}

void ClientApiTest::testDownsamplingDisabled()
{
    MagnetometerSensorChannelInterface* magnetometer1 = MagnetometerSensorChannelInterface::interface("magnetometersensor");
    QVERIFY2(magnetometer1 && magnetometer1->isValid(), "Could not get magnetometer sensor channel");
    SampleCollector client1(*magnetometer1, true);
    magnetometer1->setInterval(100);
    magnetometer1->start();

    MagnetometerSensorChannelInterface* magnetometer2 = MagnetometerSensorChannelInterface::interface("magnetometersensor");
    QVERIFY2(magnetometer2 && magnetometer2->isValid(), "Could not get magnetometer sensor channel");
    SampleCollector client2(*magnetometer2, true);
    magnetometer2->setInterval(1000);
    magnetometer2->setDownsampling(false);
    magnetometer2->start();

    qDebug() << "Magnetometer sensor started, waiting for 2200ms.";
    QTest::qWait(2200);

    magnetometer1->stop();
    magnetometer2->stop();

    QVERIFY(client1.getSamples().size() > 20);
    QVERIFY(abs(client1.getSamples().size() - client2.getSamples().size()) < 2);

    delete magnetometer1;
    delete magnetometer2;
}





TestClient::TestClient(AbstractSensorChannelInterface& iface, bool listenFrames) :
        dataCount(0),
        frameCount(0),
        frameDataCount(0)
{
    QString id = iface.id();
    if (id=="magnetometersensor"){
        connect(&iface, SIGNAL(dataAvailable(const MagneticField&)), this, SLOT(dataAvailable(const MagneticField&)));
        if(listenFrames)
            connect(&iface, SIGNAL(frameAvailable(const QVector<MagneticField>&)), this, SLOT(frameAvailable(const QVector<MagneticField>&)));
        return;
    }

    connect(&iface, SIGNAL(dataAvailable2(const XYZ&)), this, SLOT(dataAvailable2(const XYZ&)));
    if(listenFrames)
        connect(&iface, SIGNAL(frameAvailable2(const QVector<XYZ>&)), this, SLOT(frameAvailable2(const QVector<XYZ>&)));

}

TestClient::~TestClient()
{
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
    samples.push_back((MagneticField*)(&data));
    TestClient::dataAvailable(data);
}

void SampleCollector::frameAvailable(const QVector<MagneticField>& frame)
{
    foreach(const MagneticField& data, frame)
    {
        samples.push_back((MagneticField*)(&data));
    }
    TestClient::frameAvailable(frame);
}
void SampleCollector::dataAvailable2(const XYZ& data)
{
    samples.push_back((XYZ*)(&data));
    TestClient::dataAvailable2(data);
}

void SampleCollector::frameAvailable2(const QVector<XYZ>& frame)
{
    foreach(const XYZ& data, frame)
    {
        samples.push_back((XYZ*)(&data));
    }
    TestClient::frameAvailable2(frame);
}

QTEST_MAIN(ClientApiTest)
