#ifndef SENSORAPITEST_H
#define SENSORAPITEST_H

#include <QTest>

#include "sensord/sensormanager.h"

class SensorApiTest : public QObject
{
    Q_OBJECT

private:
    static QString bluetoothAddress1;
    static QString bluetoothAddress2;

private slots:
    void initTestCase();
    /*
    void init()
     { qDebug("called before each testfunction is executed"); }
    */

#ifdef USE_ARIANE
    void testDeviceAdaptorRegistration();
    void testDeviceAdaptorManagement();
    void testSensorManagement();

    void testSensorReconnectionAndChange();
    void testFilteredSensor();
    void testArianeSensorInterval();
    void testArianeSensor();
    void testArianeTapSensor();
    void testMultipleArianeSensor();
    void testArianeTwoSensorBoxes();
    void testSuomenlinna();
#endif

#ifdef USE_MOTION_BAND
    void testRawOrientationSensor();
#endif

#ifdef USE_INTERNAL
    void testAccelerometerSensor();
    void testProximitySensor();
    void testOrientationSensor();
#ifdef USE_COMPASS
    void testCompassSensor();
#endif
    void testALSSensor();
    //void testTapSensor();
#endif

    /*
    void cleanup()
     { qDebug("called after every testfunction"); }
    void cleanupTestCase()
     { qDebug("called after all tests have finished"); }
     */
};

#endif // SENSORAPITEST_H
