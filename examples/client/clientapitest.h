#ifndef CLIENT_API_TEST_H
#define CLIENT_API_TEST_H

#include <QTest>

/*
  Test Cases still to be added:
  - using a already released interface
  - race conditions with respect to releasing interfaces
  - finding / searching interfaces
*/

class ClientApiTest : public QObject
{
     Q_OBJECT

private slots:
    void initTestCase();
    /*
    void init()
     { qDebug("called before each testfunction is executed"); }
    */

    void testSensorManagement();

#ifdef USE_ARIANE
    void testFilteredSensorControl();
    void testArianeTapSensor();
    void testArianeSensorControl();
    void testArianeMultipleSensorControl();
    void testArianeTwoSensorBoxes();
    void testArianeVibra();
    void testArianeGestures();
#endif

#ifdef USE_INTERNAL
#ifdef USE_COMPASS
    void testCompassSensorControl();
#endif
    void testAccelerometerSensorControl();
    void testOrientationSensorControl();
    void testALSSensorControl();
    void testTapSensorControl();
#endif

    /*
    void cleanup()
     { qDebug("called after every testfunction"); }
    void cleanupTestCase()
     { qDebug("called after all tests have finished"); }
     */
};

#endif // CLIENT_API_TEST_H
