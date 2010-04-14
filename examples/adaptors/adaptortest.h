#ifndef ADAPTORTEST_H
#define ADAPTORTEST_H

#include <QTest>

#ifdef USE_ARIANE
#include <usb.h>
#include "adaptors/arianeadaptor/arianeadaptor.h"
#endif

class AdaptorTest : public QObject
{
     Q_OBJECT

public:
    AdaptorTest()
#ifdef USE_ARIANE
     : dev_(NULL), connectionInitialized_(false)
#endif
    {
    }

#ifdef USE_ARIANE
    //static const int LOCATION                   = ARIANE_DEVICE_LOCATION_HEADSET; // head phones
    static const int LOCATION                     = ARIANE_DEVICE_LOCATION_WRIST;   // sensor box
    //static const int LOCATION                   = ARIANE_DEVICE_LOCATION_HOST;    // USB add-on

    static const int WRONG_SENSOR_ID = -1;

    static const int SYSTEM_SENSOR_ID;
    static const int ORIENTATION_SENSOR_ID;
    static const int THERMISTOR_SENSOR_ID;
    static const int MAGNETOMETER_SENSOR_ID;
    static const int GYROSCOPE_SENSOR_ID;
    static const int BAROMETER_SENSOR_ID;
    static const int ACCELEROMETER_2G_SENSOR_ID;
    static const int ACCELEROMETER_4G_SENSOR_ID;
    static const int ACCELEROMETER_8G_SENSOR_ID;
    static const int DUMMY_SENSOR_0_ID;
    static const int DUMMY_SENSOR_1_ID;
#endif

protected:
#ifdef USE_ARIANE
    void closeUsbConnection();
    void initUsbConnection();

    ariane_sensordev_t* dev_;
    bool                connectionInitialized_;
#endif

private slots:
    //void initTestCase() { qDebug("Called before any testfunction is executed"); }
    //void init() { qDebug("called before each testfunction is executed"); }

#ifdef USE_MOTION_BAND
    void testMotionBandDeviceAdaptor();
#endif

#ifdef USE_ARIANE
    void testArianeSyncApiOpenCloseSingleSensor();
    void testArianeSyncApiOpenCloseMultipleSensors();

    void testArianeSyncApiStartStopSingleSensor();
    void testArianeSyncApiStartStopMultipleSensors();

    void testArianeSyncApiVersionVerification();
    void testArianeSyncApiParamManip();
    void testArianeSyncApiPropertyRetrieval();
    void testArianeSyncApiSubSysCap();

    void testArianeSyncApiSensorDependencies();
    void testArianeSyncApiSubSysFilter();
    void testArianeSyncApiBinaryBlockTransfer();
    void testArianeSyncApiEnumerateEverything();

    void testArianeAdaptedSensorEntry();
    void testArianeAdaptorRequestAndRelease();
    void testArianeDeviceAdaptorStartStop();
    void testArianeDeviceAdaptorProperties();
    void testArianeDeviceAdaptor();
    void testArianeDeviceAdaptorVariantMode();
    void testArianeDeviceAdaptorMultipleVariant();
    void testArianeDeviceAdaptorIntervalProperty();
#endif

#ifdef USE_INTERNAL
    void testAccelerometerAdaptor();
    void testMagnetometerAdaptor();
    void testALSAdaptor();
    void testTapAdaptor();
    void testTouchAdaptor();
    void testKeyboardSliderAdaptor();
    void testProximityAdaptor();
#endif

#ifdef USE_ARIANE
    void cleanup();
#else
    //void cleanup() { qDebug("Called after every testfunction"); }
#endif
    //void cleanupTestCase() { qDebug("Called after all tests have finished"); }
};

#endif // ADAPTORTEST_H
