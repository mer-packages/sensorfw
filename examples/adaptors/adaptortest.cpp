#include <QtDebug>
#include <QTest>
#include <QVariant>

#include <cerrno>

#include "sensord/sensormanager.h"

#ifdef USE_INTERNAL
#include "adaptors/accelerometeradaptor/accelerometeradaptor.h"
#include "adaptors/magnetometeradaptor/magnetometeradaptor.h"
#include "adaptors/alsadaptor/alsadaptor.h"
#include "adaptors/touchadaptor/touchadaptor.h"
#include "adaptors/tapadaptor/tapadaptor.h"
#include "adaptors/kbslideradaptor/kbslideradaptor.h"
#include "adaptors/proximityadaptor/proximityadaptor.h"
#endif

#ifdef USE_MOTION_BAND
#include "adaptors/motionbandadaptor.h"
#endif

#include "adaptortest.h"

//#define TEST_ERROR_HANDLING
//#define OPEN_BY_TYPE

#ifdef USE_ARIANE
const int AdaptorTest::SYSTEM_SENSOR_ID           = ARIANE_SID(0, LOCATION, ARIANE_MTYPE_SYSTEM);
const int AdaptorTest::ORIENTATION_SENSOR_ID      = ARIANE_SID(0, LOCATION, ARIANE_MTYPE_ORIENTATION);
const int AdaptorTest::THERMISTOR_SENSOR_ID       = ARIANE_SID(0, LOCATION, ARIANE_MTYPE_TEMPERATURE);
const int AdaptorTest::MAGNETOMETER_SENSOR_ID     = ARIANE_SID(0, LOCATION, ARIANE_MTYPE_MAGNETIC_FIELD);
const int AdaptorTest::GYROSCOPE_SENSOR_ID        = ARIANE_SID(0, LOCATION, ARIANE_MTYPE_ANGULAR_RATE);
const int AdaptorTest::BAROMETER_SENSOR_ID        = ARIANE_SID(0, LOCATION, ARIANE_MTYPE_PRESSURE);
const int AdaptorTest::ACCELEROMETER_2G_SENSOR_ID = ARIANE_SID(0, LOCATION, ARIANE_MTYPE_ACCELERATION);
const int AdaptorTest::ACCELEROMETER_4G_SENSOR_ID = ARIANE_SID(1, LOCATION, ARIANE_MTYPE_ACCELERATION);
const int AdaptorTest::ACCELEROMETER_8G_SENSOR_ID = ARIANE_SID(2, LOCATION, ARIANE_MTYPE_ACCELERATION);
const int AdaptorTest::DUMMY_SENSOR_0_ID          = ARIANE_SID(0, LOCATION, ARIANE_MTYPE_TESTPATTERN);
const int AdaptorTest::DUMMY_SENSOR_1_ID          = ARIANE_SID(1, LOCATION, ARIANE_MTYPE_TESTPATTERN);
#endif

#ifdef USE_ARIANE
void AdaptorTest::cleanup()
{
    //qDebug("called after every testfunction");
    if (dev_) closeUsbConnection();
    QVERIFY( dev_ == NULL );
}

void AdaptorTest::initUsbConnection()
{
    if (dev_)
    {
        int rc = ariane_disconnect(dev_);
        ArianeAdaptor::printSubsystemErrorStr(rc);
        QCOMPARE( rc, 0 );

        dev_ = NULL;
    }

    QVERIFY( dev_ == NULL );

    int rc = ariane_init();
    ArianeAdaptor::printSubsystemErrorStr(rc);
    QCOMPARE( rc, 0 );

    rc = ariane_usb_connect(&dev_);
    if ( rc == -ENODEV )
    {
        qDebug() << "No USB device found...";
    }
    ArianeAdaptor::printSubsystemErrorStr(rc);
    QCOMPARE( rc, 0 );

    connectionInitialized_ = true;
}

void AdaptorTest::closeUsbConnection()
{
    if (dev_)
    {
        int rc = ariane_disconnect(dev_);
        ArianeAdaptor::printSubsystemErrorStr(rc);
        QCOMPARE( rc, 0 );

        dev_ = NULL;

        connectionInitialized_ = false;
    }
}
#endif

#ifdef USE_MOTION_BAND
void AdaptorTest::testMotionBandDeviceAdaptor()
{
    MotionBandAdaptor* adaptor = dynamic_cast<MotionBandAdaptor*>(MotionBandAdaptor::factoryMethod("motionbandadaptor"));
    QVERIFY( adaptor );

    // set some DA specific properties
    const QString DEVICE_FILE_NAME = "/dev/rfcomm0";
    adaptor->setDeviceFileName(DEVICE_FILE_NAME);
    QCOMPARE(adaptor->deviceFileName(), DEVICE_FILE_NAME);

    const QString CALIB_FILE_NAME = "mb72.cal";
    adaptor->setCalibrationFileName(CALIB_FILE_NAME);
    QCOMPARE( adaptor->calibrationFileName(), CALIB_FILE_NAME);

    const unsigned SAMPLE_INTERVAL = 1000;
    adaptor->setInterval(SAMPLE_INTERVAL);
    QCOMPARE( adaptor->interval(), SAMPLE_INTERVAL);

    bool ok = adaptor->startAdaptor();
    QVERIFY(ok);

    const int INTERVAL = 15000;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    adaptor->stopAdaptor();

    delete adaptor;
}
#endif

#ifdef USE_ARIANE
void printParamDescription(const ariane_sensor_parameter_t& parameterDescription)
{
    qDebug() << "Parameter Description";
    qDebug() << "description      =" << parameterDescription.description;
    qDebug() << "param id         =" << parameterDescription.paramid;
    qDebug() << "min value        =" << parameterDescription.min;
    qDebug() << "max value        =" << parameterDescription.max;
    qDebug() << "flags            =" << parameterDescription.flags;
}

void printSensorProperties(const ariane_sensor_properties_t& sensorProperties)
{
    qDebug() << "Sensor Properties";
    qDebug() << "data format      =" << sensorProperties.datafmt;
    qDebug() << "sample rate [Hz] =" << sensorProperties.sample_rate;
    qDebug() << "resolution bits  =" << sensorProperties.resolution_bits;
    qDebug() << "sample channels  =" << sensorProperties.sample_channels;
    qDebug() << "bit flags        =" << sensorProperties.flags;

    Q_ASSERT( sensorProperties.sample_channels <= ARIANE_MAX_CHANNELS );
    for (int i = 0; i < sensorProperties.sample_channels; i++)
    {
        qDebug() << "  conversions[" << i << "].offset =" << sensorProperties.conversions[i].offset;
        qDebug() << "  conversions[" << i << "].scale  =" << sensorProperties.conversions[i].scale;
    }
}


void AdaptorTest::testArianeSyncApiOpenCloseSingleSensor()
{
    ariane_sensor_t* ctlSensor1 = 0;
    ariane_access_mode_t accessMode;
    const ariane_sid_t sensorId = ORIENTATION_SENSOR_ID;

#if 0
#ifdef TEST_ERROR_HANDLING
    accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, (ariane_mtype_t)ARIANE_SID_GET_MTYPE(sensorId), ARIANE_SENSOR_ACCESS_MODE_ANY, &ctlSensor1);
    QVERIFY(accessMode < 0 ); // does not check existence
#endif
#endif
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    // open non existing sensor id (should fail)
#ifdef TEST_ERROR_HANDLING
    accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, WRONG_SENSOR_ID, ARIANE_SENSOR_ACCESS_MODE_ANY, &ctlSensor1);
    QVERIFY(accessMode == ARIANE_SENSOR_ACCESS_MODE_DENIED); // does not check existence
#endif

    // open existing sensor id (should be successfull)
    ariane_sensor_t* ctlSensor2 = 0;
#ifdef OPEN_BY_TYPE
    accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, (ariane_mtype_t)ARIANE_SID_GET_MTYPE(sensorId), ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor2);
#else
    accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, sensorId, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor2);
#endif
    QVERIFY( ctlSensor2 );
    QVERIFY( accessMode == ARIANE_SENSOR_ACCESS_MODE_CONTROL );

#ifdef TEST_ERROR_HANDLING
    // claim again as control (should fail)
    ariane_sensor_t* ctlSensor3 = 0;
#ifdef OPEN_BY_TYPE
    accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, (ariane_mtype_t)ARIANE_SID_GET_MTYPE(sensorId), ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor3);
#else
    accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, sensorId, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor3);
#endif
    QVERIFY( accessMode == ARIANE_SENSOR_ACCESS_MODE_DENIED );
#endif

    // claim again as monitor (should be succesfull)
    ariane_sensor_t* monSensor = 0;
#ifdef OPEN_BY_TYPE
    accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, (ariane_mtype_t)ARIANE_SID_GET_MTYPE(sensorId), ARIANE_SENSOR_ACCESS_MODE_MONITOR, &monSensor);
#else
    accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, sensorId, ARIANE_SENSOR_ACCESS_MODE_MONITOR, &monSensor);
#endif
    QVERIFY( monSensor );
    QVERIFY(accessMode == ARIANE_SENSOR_ACCESS_MODE_MONITOR);

    int rc = 0;
#ifdef TEST_ERROR_HANDLING
    // close non-open sensor (should fail)
    rc = ariane_sensor_close(NULL); // causes seg fault?
    QVERIFY( rc < 0 );
#endif

    // close sensor that could not be opened (should fail)
    rc = ariane_sensor_close(ctlSensor1);
    QVERIFY( rc < 0 );

    // close sensor that was opened succesfull (should be succesfull)
    rc = ariane_sensor_close(ctlSensor2);
    QCOMPARE( rc, 0 );

#ifdef TEST_ERROR_HANDLING
    // close sensor that could not be opened (should fail)
    rc = ariane_sensor_close(ctlSensor3);
    QVERIFY( rc < 0 );
#endif

    // close sensor that was opened succesfull (should be succesfull)
    rc = ariane_sensor_close(monSensor);
    QCOMPARE( rc, 0 );

    closeUsbConnection();
}

void AdaptorTest::testArianeSyncApiOpenCloseMultipleSensors()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    ariane_sensor_t* ctlSensor1 = 0;
    ariane_access_mode_t accessMode;
#ifdef OPEN_BY_TYPE
    accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, ARIANE_MTYPE_MAGNETIC_FIELD, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor1);
#else
    accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, MAGNETOMETER_SENSOR_ID, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor1);
#endif
    QVERIFY(accessMode == ARIANE_SENSOR_ACCESS_MODE_CONTROL);

    ariane_sensor_t* ctlSensor2 = 0;
#ifdef OPEN_BY_TYPE
    accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, ARIANE_MTYPE_ANGULAR_RATE, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor2);
#else
    accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, GYROSCOPE_SENSOR_ID, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor2);
#endif
    QVERIFY(accessMode == ARIANE_SENSOR_ACCESS_MODE_CONTROL);

    ariane_sensor_t* ctlSensor3 = 0;
#ifdef OPEN_BY_TYPE
    accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, ARIANE_MTYPE_ACCELERATION, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor3);
#else
    accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, ACCELEROMETER_8G_SENSOR_ID, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor3);
#endif
    QVERIFY(accessMode == ARIANE_SENSOR_ACCESS_MODE_CONTROL);

    char descriptionStr[ARIANE_MAX_SENSOR_DESCR_LEN];
    int rc = ariane_sensor_get_description(ctlSensor1, descriptionStr);
    QCOMPARE( rc, 0 );
    QCOMPARE( QString(descriptionStr), QString("3-axis magnetometer") );

    rc = ariane_sensor_get_description(ctlSensor2, descriptionStr);
    QCOMPARE( rc, 0 );
    QCOMPARE( QString(descriptionStr), QString("3-axis 600deg/s gyroscope") );

    rc = ariane_sensor_get_description(ctlSensor3, descriptionStr);
    QCOMPARE( rc, 0 );
    QCOMPARE( QString(descriptionStr), QString("3-axis 8G accelerometer") );

    rc = ariane_sensor_close(ctlSensor1);
    QCOMPARE( rc, 0 );

    rc = ariane_sensor_close(ctlSensor2);
    QCOMPARE( rc, 0 );

    rc = ariane_sensor_close(ctlSensor3);
    QCOMPARE( rc, 0 );

    closeUsbConnection();
}

class TestEventHandler {
public:
    TestEventHandler() : eventCount_(0) {}

    static void staticEventHandler(const ariane_event_t* event, void *param)
    {
        TestEventHandler* handler = static_cast<TestEventHandler*>( param );
        Q_ASSERT( handler );

        handler->eventHandler(event);
    }

    void eventHandler(const ariane_event_t* event)
    {
        QVERIFY( event );
        QVERIFY( event->sensor );
#if 0
        qDebug() << __PRETTY_FUNCTION__;
        if (event)
        {
            qDebug() << "timestamp          =" << event->timestamp;
            if (event->sensor)
            {
                qDebug() << "sensor.dev         =" << event->sensor->dev;
                qDebug() << "sensor.session     =" << event->sensor->session;
                qDebug() << "sensor.sid         =" << event->sensor->sid;
                qDebug() << "sensor.access_mode =" << event->sensor->access_mode;
                qDebug() << "sensor.datafmt     =" << event->sensor->datafmt;

            }
            else
            {
                qDebug() << "event->sensor == NULL";
            }

            qDebug() << "datafmt            =" << event->datafmt;
        }
        else
        {
            qDebug() << "event == NULL";
        }
#endif
        QVERIFY ( event->sensor->datafmt == event->datafmt );

        eventCount_++;
   }

    volatile int eventCount_;
};

void AdaptorTest::testArianeSyncApiStartStopSingleSensor()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    ariane_sensor_t* ctlSensor = 0;
#ifdef OPEN_BY_TYPE
    ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, ARIANE_MTYPE_ORIENTATION, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor);
#else
    ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, ORIENTATION_SENSOR_ID, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor);
#endif
    QVERIFY( ctlSensor );
    QVERIFY( accessMode == ARIANE_SENSOR_ACCESS_MODE_CONTROL );

    TestEventHandler handler;
    int rc = ariane_sensor_start(ctlSensor, TestEventHandler::staticEventHandler, &handler);
    QCOMPARE( rc, 0 );

    const int INTERVAL = 500;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );
    qDebug ( "Done waiting..." );
    QVERIFY( handler.eventCount_ > 0 );
    qDebug() << "handler.eventCount_" << handler.eventCount_;

    rc = ariane_sensor_stop(ctlSensor);
    QCOMPARE( rc, 0 );

    rc = ariane_sensor_close(ctlSensor);
    QCOMPARE( rc, 0 );

    closeUsbConnection();
}

void AdaptorTest::testArianeSyncApiStartStopMultipleSensors()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    ariane_sensor_t* ctlSensor1 = 0;
#ifdef OPEN_BY_TYPE
    ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, ARIANE_MTYPE_ANGULAR_RATE, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor1);
#else
    ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, GYROSCOPE_SENSOR_ID, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor1);
#endif
    QVERIFY(accessMode == ARIANE_SENSOR_ACCESS_MODE_CONTROL);

    ariane_sensor_t* ctlSensor2 = 0;
#ifdef OPEN_BY_TYPE
    accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, ARIANE_MTYPE_PRESSURE, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor2);
#else
    accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, BAROMETER_SENSOR_ID, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor2);
#endif
    QVERIFY(accessMode == ARIANE_SENSOR_ACCESS_MODE_CONTROL);

    ariane_sensor_t* ctlSensor3 = 0;
#ifdef OPEN_BY_TYPE
    accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, ARIANE_MTYPE_ACCELERATION, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor3);
#else
    accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, ACCELEROMETER_2G_SENSOR_ID, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor3);
#endif
    QVERIFY(accessMode == ARIANE_SENSOR_ACCESS_MODE_CONTROL);

    TestEventHandler handler1;
    int rc = ariane_sensor_start(ctlSensor1, TestEventHandler::staticEventHandler, &handler1);
    QCOMPARE( rc, 0 );

    TestEventHandler handler2;
    rc = ariane_sensor_start(ctlSensor2, TestEventHandler::staticEventHandler, &handler2);
    QCOMPARE( rc, 0 );

    TestEventHandler handler3;
    rc = ariane_sensor_start(ctlSensor3, TestEventHandler::staticEventHandler, &handler3);
    QCOMPARE( rc, 0 );

    const int INTERVAL = 1000;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );
    qDebug ( "Done waiting..." );

    qDebug() << "handler1.eventCount_" << handler1.eventCount_;
    qDebug() << "handler2.eventCount_" << handler2.eventCount_;
    qDebug() << "handler3.eventCount_" << handler3.eventCount_;

    QVERIFY( handler1.eventCount_ > 0 );
    QVERIFY( handler2.eventCount_ > 0 );
    QVERIFY( handler3.eventCount_ > 0 );

    rc = ariane_sensor_stop(ctlSensor1);
    QCOMPARE( rc, 0 );

    rc = ariane_sensor_stop(ctlSensor2);
    QCOMPARE( rc, 0 );

    rc = ariane_sensor_stop(ctlSensor3);
    QCOMPARE( rc, 0 );

    rc = ariane_sensor_close(ctlSensor1);
    QCOMPARE( rc, 0 );

    rc = ariane_sensor_close(ctlSensor2);
    QCOMPARE( rc, 0 );

    rc = ariane_sensor_close(ctlSensor3);
    QCOMPARE( rc, 0 );

    closeUsbConnection();
}

void AdaptorTest::testArianeSyncApiVersionVerification()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    int version = ariane_sensordev_get_version(dev_);
    QVERIFY(version >= 9);

    int minorVersion = version & 0x00ff;
    int majorVersion = (version >> 8 )& 0x00ff;

    qDebug() << "SW Firmware version:" << majorVersion << "." << minorVersion;

    //QVERIFY(majorVersion >= 0);
    //QVERIFY(minorVersion >= 9);

    closeUsbConnection();
}

void AdaptorTest::testArianeSyncApiParamManip()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    ariane_sensor_t* ctlSensor = 0;
#ifdef OPEN_BY_TYPE
    ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, ARIANE_MTYPE_ORIENTATION, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor);
#else
    ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, ORIENTATION_SENSOR_ID, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &ctlSensor);
#endif
    QVERIFY(accessMode == ARIANE_SENSOR_ACCESS_MODE_CONTROL);

    ariane_paramid_t parameterIdList[ARIANE_MAX_PARAMETERS];
    int numParams = ariane_sensor_get_parameter_ids(ctlSensor, parameterIdList);
    QCOMPARE(numParams, 1);

    ariane_paramid_t parameterId = parameterIdList[0]; // first and only parameter of this sensor
    ariane_sensor_parameter_t parameterDescription;
    int rc = ariane_sensor_get_parameter_description(ctlSensor, parameterId, &parameterDescription);
    QCOMPARE(rc, 0);

    /*
    Sensor Parameters:
    param id	 = 0
    description	 = Yaw angle mode
    min value	 = 0
    max value	 = 1
    flags	 = 2
    value	 = 0
    */

    qDebug() << "parameter_description.description:" << parameterDescription.description;
    QVERIFY( parameterDescription.paramid == 0 );
    QVERIFY( parameterDescription.min == 0 );
    QVERIFY( parameterDescription.max == 1 );
    QVERIFY( parameterDescription.max > parameterDescription.min );
    QVERIFY( parameterDescription.flags == 2 );
    QCOMPARE( QString(parameterDescription.description), QString("Yaw angle mode") );

    uint32_t value = parameterDescription.max + 1;
    rc = ariane_sensor_get_parameter_value(ctlSensor, parameterId, &value);
    QCOMPARE(rc, 0);
    QVERIFY( parameterDescription.min <= value );
    QVERIFY( value <= parameterDescription.max );

#ifdef TEST_ERROR_HANDLING
    // non existing parameter
    rc = ariane_sensor_set_parameter_value(ctlSensor, parameterId + 1, &value);
    QVERIFY(rc < 0);
#endif

#ifdef TEST_ERROR_HANDLING
    // existing parameter, but out of range (should fail)
    value = parameterDescription.max + 1;
    rc = ariane_sensor_set_parameter_value(ctlSensor, parameterId, &value);
    QVERIFY(rc < 0);
#endif

    // existing parameter, maximum value (should be succesfull)
    value = parameterDescription.max;
    qDebug() << "Setting parameter to:" << value;
    rc = ariane_sensor_set_parameter_value(ctlSensor, parameterId, &value);
    QCOMPARE(rc, 0);

    rc = ariane_sensor_get_parameter_value(ctlSensor, parameterId, &value);
    QCOMPARE(rc, 0);
    qDebug() << "Receiving parameter:" << value;
    QCOMPARE(value, parameterDescription.max);

    // existing parameter, minimum value (should be succesfull)
    value = parameterDescription.min;
    qDebug() << "Setting parameter to:" << value;
    rc = ariane_sensor_set_parameter_value(ctlSensor, parameterId, &value);
    QCOMPARE(rc, 0);

    rc = ariane_sensor_get_parameter_value(ctlSensor, parameterId, &value);
    QCOMPARE(rc, 0);
    qDebug() << "Receiving parameter:" << value;
    QCOMPARE(value, parameterDescription.min);

    rc = ariane_sensor_close(ctlSensor);
    QCOMPARE(rc, 0);

#ifdef TEST_ERROR_HANDLING
    // existing parameter, but closed session
    rc = ariane_sensor_set_parameter_value(ctlSensor, parameterId, &value);
    QVERIFY(rc < 0);
#endif

    closeUsbConnection();
}

void AdaptorTest::testArianeSyncApiPropertyRetrieval()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    ariane_sid_t sensorsList[ARIANE_MAX_SENSORS];
    int numSensors = ariane_sensordev_get_sensors(dev_, sensorsList);
    QVERIFY( numSensors > 0 );

    QMap<uint16_t, ariane_sensor_properties_t> sensorMap;
    for (int i = 1; i < numSensors; i++)
    {
        QVERIFY( i < ARIANE_MAX_SENSORS );

        ariane_sensor_t* sensor = 0;
        ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_,
                                                                                      0,
                                                                                      sensorsList[i],
                                                                                      ARIANE_SENSOR_ACCESS_MODE_ANY,
                                                                                      &sensor);
        QVERIFY ( sensor  );
        QVERIFY ( accessMode >= 0 );
        QVERIFY ( accessMode != ARIANE_SENSOR_ACCESS_MODE_DENIED );

        ariane_sensor_properties_t sensorProps;
        int rc = ariane_sensor_get_properties(sensor, &sensorProps);

        if ( rc == 0 )
        {
            ArianeAdaptor::printSubsystemErrorStr(rc);
            QCOMPARE( rc, 0 );

            sensorMap[sensorsList[i]] = sensorProps;

            rc = ariane_sensor_close(sensor);
            ArianeAdaptor::printSubsystemErrorStr(rc);
            QCOMPARE( rc, 0 );

            //printSensorProperties(sensorProps);
        }
    }

    /*
    Orientation (Tom) (2080) Sensor Properties:
    sample rate [Hz] = 20
    resolution bits  = 16
    sample channels  = 3
    bit flags        = 2
    unit conversion of sample 0 : a = 4.6006e-41 , b = 0
    unit conversion of sample 1 : a = 4.6006e-41 , b = 0
    unit conversion of sample 2 : a = 4.6006e-41 , b = 0
    */

    QVERIFY( sensorMap[ORIENTATION_SENSOR_ID].datafmt == ARIANE_EVENT_DATAFMT_XYZ16 );
    QVERIFY( sensorMap[ORIENTATION_SENSOR_ID].sample_rate == 20 );
    QVERIFY( sensorMap[ORIENTATION_SENSOR_ID].resolution_bits == 16 );
    QVERIFY( sensorMap[ORIENTATION_SENSOR_ID].sample_channels == 3 );
    QVERIFY( sensorMap[ORIENTATION_SENSOR_ID].flags == 2 );

    /*
    Barometer VTI SCP1000 (544) Sensor Properties:
    sample rate [Hz] = 2
    resolution bits  = 17
    sample channels  = 1
    bit flags        = 0
    unit conversion of sample 0 : a = 4.60046e-41 , b = 0
    */

    QVERIFY( sensorMap[BAROMETER_SENSOR_ID].datafmt == ARIANE_EVENT_DATAFMT_U32 );
    QVERIFY( sensorMap[BAROMETER_SENSOR_ID].sample_rate == 2 );
    QVERIFY( sensorMap[BAROMETER_SENSOR_ID].resolution_bits == 17 );
    QVERIFY( sensorMap[BAROMETER_SENSOR_ID].sample_channels == 1 );
    QVERIFY( sensorMap[BAROMETER_SENSOR_ID].flags == 0 );

    closeUsbConnection();
}

void AdaptorTest::testArianeSyncApiSubSysCap()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    ariane_sid_t sensorsList[ARIANE_MAX_SENSORS];
    int numSensors = ariane_sensordev_get_sensors(dev_, sensorsList);
    QVERIFY( numSensors > 0 );

    char descriptionStr[ARIANE_MAX_SENSOR_DESCR_LEN];

    // TODO: make this robust against firmware updates of the sensor hardware
    const char* sensorUsbAddonNames[] = {
            "Thermistor",
            "System control",
            "Orientation (Tom)",
            "3-axis magnetometer",
            "3-axis 600deg/s gyroscope",
            "Dummy sensor for testing",
            "Dummy sensor for testing",
            "Barometer VTI SCP1000",
            "3-axis 2G accelerometer",
            "3-axis 4G accelerometer",
            "3-axis 8G accelerometer"
        };

    // TODO: make this robust against firmware updates of the sensor hardware
    const char* sensorSensorBoxNames[] = {
            "Haptic control",
            "Push buttons",
            "Thermistor",
            "Tap detection (SW)",
            "System control",
            "Orientation (Tom)",
            "3-axis magnetometer",
            "3-axis 600deg/s gyroscope",
            "Gyro \"orientation\" (relative)",
            "Dummy sensor for testing",
            "Dummy sensor for testing",
            "Barometer VTI SCP1000",
            "3-axis 2G accelerometer",
            "3-axis 4G accelerometer",
            "3-axis 8G accelerometer"
        };

    for (unsigned i = 0; i < (unsigned)numSensors; i++)
    {
        QVERIFY( i < ARIANE_MAX_SENSORS );

        ariane_sensor_t* sensor = 0;
        ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, sensorsList[i], ARIANE_SENSOR_ACCESS_MODE_ANY, &sensor);
        QVERIFY ( accessMode != ARIANE_SENSOR_ACCESS_MODE_DENIED );

        int rc = ariane_sensor_get_description(sensor, descriptionStr);
        QCOMPARE( rc, 0 );
        qDebug() << "Sensor ID:" << sensorsList[i] << descriptionStr;

        switch ( (sensorsList[i] >> 4) & 0x000f )
        {
            case ARIANE_DEVICE_LOCATION_WRIST :
                QVERIFY( i < sizeof(sensorSensorBoxNames) / sizeof(char*) );
                QCOMPARE( QString(descriptionStr), QString(sensorSensorBoxNames[i]) );
            break;
            case ARIANE_DEVICE_LOCATION_HOST :
                QVERIFY( i < sizeof(sensorUsbAddonNames) / sizeof(char*) );
                QCOMPARE( QString(descriptionStr), QString(sensorUsbAddonNames[i]) );
            break;
            case ARIANE_DEVICE_LOCATION_HEADSET :
            default:
                QVERIFY( false );
            break;
        }

        rc = ariane_sensor_close(sensor);
        QCOMPARE( rc, 0 );
    }

    closeUsbConnection();
}

void AdaptorTest::testArianeSyncApiSensorDependencies()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    ariane_sid_t sensorsList[ARIANE_MAX_SENSORS];
    int numDeps = ariane_sensordev_get_all_dependencies(dev_, ORIENTATION_SENSOR_ID, sensorsList);
    QVERIFY( numDeps >= 0 );

    QSet<int> sensorSet;
    for (int i = 0; i < numDeps; i++)
    {
        QVERIFY( i < ARIANE_MAX_SENSORS );

        //qDebug() << "sensorsList[i]" << sensorsList[i];
        sensorSet.insert( sensorsList[i] );
    }

    QCOMPARE( sensorSet.size(), 5 );

    QVERIFY( sensorSet.contains(MAGNETOMETER_SENSOR_ID) );
    QVERIFY( sensorSet.contains(GYROSCOPE_SENSOR_ID) );
    QVERIFY( sensorSet.contains(ACCELEROMETER_2G_SENSOR_ID) );
    QVERIFY( sensorSet.contains(ACCELEROMETER_4G_SENSOR_ID) );
    QVERIFY( sensorSet.contains(ACCELEROMETER_8G_SENSOR_ID) );

    closeUsbConnection();
}

void AdaptorTest::testArianeSyncApiSubSysFilter()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    /*
    int list_subsystem_filters(int id, int *filters); // list all available filters
    int get_filter_description(int id, int filter_id, char *text); // get text-description of filter
    int instantiate_subsystem_filter(int id, int filter_id, int connect_sid, int *new_sid); // create new instance of filter and get its new sid, a new session must be opened to use it
    */

    QVERIFY( false ); // test not implemented

    closeUsbConnection();
}

void AdaptorTest::testArianeSyncApiBinaryBlockTransfer()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    ariane_sensor_t* sensor = 0;
#ifdef OPEN_BY_TYPE
    ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open_by_type(dev_, 0, ARIANE_MTYPE_TESTPATTERN, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &sensor);
#else
    ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, DUMMY_SENSOR_0_ID, ARIANE_SENSOR_ACCESS_MODE_CONTROL, &sensor);
#endif
    QVERIFY ( accessMode == ARIANE_SENSOR_ACCESS_MODE_CONTROL );

    uint32_t blockSize;
    int rc = ariane_sensor_get_parameter_value(sensor, ARIANE_PARAMID_DATA_BLOCK_SIZE, &blockSize);
    QCOMPARE(rc, 0);

    uint32_t blockSizeVerify =ariane_sensor_get_binary_block_size(sensor);
    QCOMPARE(blockSize, blockSizeVerify);

    uint32_t transferSize = qMin((uint32_t)ARIANE_MAX_DATA_BLOCK_LEN, blockSize);
    //qDebug() << "Sensor's data block size =" << blockSize << "(transferSize =" << transferSize << ")";

    uint8_t inBuffer[ARIANE_MAX_DATA_BLOCK_LEN];
    for (int i = 0; i < ARIANE_MAX_DATA_BLOCK_LEN; i++)
    {
        inBuffer[i] = i % 256;
    }

    uint32_t offset = 0;
    rc = ariane_sensor_write_binary_block(sensor, offset, inBuffer, transferSize);
    QCOMPARE( rc,  0);

    uint8_t outBuffer[ARIANE_MAX_DATA_BLOCK_LEN];
    memset(outBuffer, 0xff, ARIANE_MAX_DATA_BLOCK_LEN);

   QVERIFY( memcmp( inBuffer, outBuffer, transferSize ) != 0 );

    rc = ariane_sensor_read_binary_block(sensor, offset, outBuffer, transferSize);
    QCOMPARE( rc, 0 );

    //for (uint32_t i = 0; i< transferSize; i++) qDebug() << inBuffer[i] << "=="<< outBuffer[i];

    QVERIFY( memcmp( inBuffer, outBuffer, transferSize ) == 0 );

    rc = ariane_sensor_close(sensor);
    QCOMPARE( rc, 0 );

    closeUsbConnection();
}

void AdaptorTest::testArianeSyncApiEnumerateEverything()
{
    initUsbConnection();
    QVERIFY ( connectionInitialized_ );

    QVERIFY ( dev_ );

    ariane_sid_t sensorsIdList[ARIANE_MAX_SENSORS];
    int numSensors = ariane_sensordev_get_sensors(dev_, sensorsIdList);
    QVERIFY( numSensors >= 0 );

    for (int i = 0; i < numSensors; i++)
    {
        ariane_sensor_t* sensor = NULL;
        ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open(dev_, 0, sensorsIdList[i], ARIANE_SENSOR_ACCESS_MODE_ANY, &sensor);
        QVERIFY ( accessMode != ARIANE_SENSOR_ACCESS_MODE_DENIED );
        if (accessMode < 0 ) qDebug() << "ERROR: " << strerror (-accessMode);
        QCOMPARE ( accessMode, (ariane_access_mode_t)0 );

        char descriptionStr[ARIANE_MAX_SENSOR_DESCR_LEN];
        int rc = ariane_sensor_get_description(sensor, descriptionStr);
        QCOMPARE ( rc, 0 );

        QString sensorDescription(descriptionStr);
        qDebug() << "Found sensor:" << sensorDescription << "(" << sensorsIdList[i] << ")";

        ariane_sensor_properties_t sensorProps;
        memset(&sensorProps, 0x00, sizeof(sensorProps));
        rc = ariane_sensor_get_properties(sensor, &sensorProps);
        QVERIFY ( rc == -ENOENT || rc >= 0 );

        if (rc != -ENOENT)
        {
            printSensorProperties(sensorProps);
        }

        ariane_sensor_parameter_t* parameterList = NULL;
        int numParams = ariane_sensor_get_parameters(sensor, &parameterList);
        QVERIFY ( numParams >= 0 );

        for (int i = 0; i < numParams; i++)
        {
            printParamDescription(parameterList[i]);

            // retrieving the actual parameter value
            uint32_t parameterValue;
            rc = ariane_sensor_get_parameter_value(sensor, parameterList[i].paramid, &parameterValue);
            QCOMPARE ( rc, 0 );
        }

        if (parameterList)
        {
            free(parameterList);
        }

        qDebug() << "ariane_sensor_close(...);";
        rc = ariane_sensor_close(sensor);
        QCOMPARE ( rc, 0 );
    }

    closeUsbConnection();
}

class TestArianeAdaptor : public ArianeAdaptor {
public:
    TestArianeAdaptor()
      : ArianeAdaptor("testarianeadaptor")
      , handleTimedXyzDataCount_(0)
      , handleTimedUnsignedCount_(0)
      , handleTimedXyzDataVariantCount_(0)
      , handleTimedUnsignedVariantCount_(0)
      {}

    virtual void handleTimedXyzData(RingBufferBase* ringBuffer, uint32_t timestamp, ariane_event_xyz16_t xyz)
    {
        Q_UNUSED ( ringBuffer );
        Q_UNUSED ( timestamp );
        Q_UNUSED ( xyz );

        //qDebug() << __PRETTY_FUNCTION__;
        handleTimedXyzDataCount_++;
    }

    virtual void handleTimedUnsigned(RingBufferBase* ringBuffer, uint32_t timestamp, unsigned data)
    {
        Q_UNUSED ( ringBuffer );
        Q_UNUSED ( timestamp );
        Q_UNUSED ( data );

        //qDebug() << __PRETTY_FUNCTION__ << timestamp << data;
        handleTimedUnsignedCount_++;
    }

    virtual void handleTimedXyzDataVariant(RingBufferBase* ringBuffer, uint32_t timestamp, ariane_event_xyz16_t xyz)
    {
        Q_UNUSED ( ringBuffer );
        Q_UNUSED ( timestamp );
        Q_UNUSED ( xyz );

        //qDebug() << __PRETTY_FUNCTION__;
        handleTimedXyzDataVariantCount_++;
    }

    virtual void handleTimedUnsignedVariant(RingBufferBase* ringBuffer, uint32_t timestamp, unsigned data)
    {
        Q_UNUSED ( ringBuffer );
        Q_UNUSED ( timestamp );
        Q_UNUSED ( data );

        //qDebug() << __PRETTY_FUNCTION__ << timestamp << data;
        handleTimedUnsignedVariantCount_++;
    }

    volatile int handleTimedXyzDataCount_;
    volatile int handleTimedUnsignedCount_;

    volatile int handleTimedXyzDataVariantCount_;
    volatile int handleTimedUnsignedVariantCount_;
};

class TestArianeAdaptedSensorEntry : public ArianeAdaptedSensorEntry {
public:
    TestArianeAdaptedSensorEntry(ArianeAdaptor& adaptor, const ariane_sensor_properties_t& sensorProps)
        : ArianeAdaptedSensorEntry(adaptor,
                                   AdaptorTest::MAGNETOMETER_SENSOR_ID,
                                   "test sensor for property manipulation",
                                   &sensorProps)
    {
    }

    // makes protected members public for testing purposes...
    bool subsystemParameter( const char * paramName, uint32_t& newValue ) { return ArianeAdaptedSensorEntry::subsystemParameter(paramName, newValue); }
    bool setSubsystemParameter( const char * paramName, uint32_t value ) { return ArianeAdaptedSensorEntry::setSubsystemParameter(paramName, value); }
};

void AdaptorTest::testArianeAdaptedSensorEntry()
{
    TestArianeAdaptor* adaptor = new TestArianeAdaptor;
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY ( ok );

    // directly starting thread without populating its regsitry
    ariane_sensor_properties_t sensorProps;
    sensorProps.datafmt         = ARIANE_EVENT_DATAFMT_XYZ16;
    sensorProps.sample_rate     = 10;
    sensorProps.resolution_bits = 16;
    sensorProps.sample_channels = 3;
    sensorProps.flags           = 0;

    for (int i = 0; i < sensorProps.sample_channels; i++)
    {
        sensorProps.conversions[i].offset = 0;
        sensorProps.conversions[i].scale  = 0;
    }

    TestArianeAdaptedSensorEntry sensorEntry(*adaptor, sensorProps);
    sensorEntry.print();

    /* info not available until sensor opened...
    QCOMPARE( sensorEntry.hwSensorId(),      uint16_t(MAGNETOMETER_SENSOR_ID) );
    QCOMPARE( sensorEntry.measurementType(), uint8_t(ARIANE_MTYPE_MAGNETIC_FIELD) );
    QCOMPARE( sensorEntry.sensorLocation(),  uint8_t(ARIANE_DEVICE_LOCATION_HOST) );
    QCOMPARE( sensorEntry.sensorNumber(),    uint8_t(0) );
    */
    QCOMPARE( sensorEntry.sampleRate(),      uint16_t(10) );
    QCOMPARE( sensorEntry.resolutionBits(),  uint8_t(16) );
    QCOMPARE( sensorEntry.sampleChannels(),  uint8_t(3) );
    QCOMPARE( sensorEntry.flags(),           uint16_t(0) );


    // initialize the property set with the interval parameter of the subsystem
    const char* paramName = "Sampling rate";
    ariane_sensor_parameter_t parameterDescription;
    parameterDescription.paramid = 256;
    parameterDescription.min     = 10;
    parameterDescription.max     = 20;
    parameterDescription.flags   = 2;
    memset(parameterDescription.description, 0x00, ARIANE_MAX_SENSOR_DESCR_LEN);
    strcpy(parameterDescription.description, paramName);

    sensorEntry.paramMap_.insert(paramName, parameterDescription);
    sensorEntry.print();

    //////////////////////////////////////////////////////////////////////////////////////////// start actual test
    // test initial reading of the value from the subsystem directly
    uint32_t value;
    ok = sensorEntry.subsystemParameter(paramName, value);
    QVERIFY( ok );

    // test setting the value of the subsystem directly
    uint32_t newValue = 15;
    ok = sensorEntry.setSubsystemParameter( paramName, newValue );
    QVERIFY( ok );

    // test reading the value from the subsystem directly (for verification)
    ok = sensorEntry.subsystemParameter(paramName, value);
    QVERIFY( ok );
    QVERIFY( value == newValue);

    // test reading of the value from the subsystem via property system hook
    QVariant rateProp = sensorEntry.property(parameterDescription.description).toInt();
    QCOMPARE( (uint32_t)rateProp.toInt(), newValue );

    // test setting the value of the subsystem via the property system hook
    newValue = 18;
    sensorEntry.setProperty( paramName, newValue );

    // test reading of the value from the subsystem via property system hook (for verification)
    rateProp = sensorEntry.property(parameterDescription.description);
    QCOMPARE( (uint32_t)rateProp.toInt(), newValue );

    adaptor->stopAdaptor();

    delete adaptor;
}

void AdaptorTest::testArianeAdaptorRequestAndRelease()
{
    QString ADAPTOR_ID = "arianeadaptor";
    SensorManager& sm = SensorManager::instance();

    ArianeAdaptor* dummy = dynamic_cast<ArianeAdaptor*>(sm.requestDeviceAdaptor(ADAPTOR_ID));
    QVERIFY ( dummy == NULL );

    sm.registerDeviceAdaptor<ArianeAdaptor>(ADAPTOR_ID);

    ArianeAdaptor* ariane1 = dynamic_cast<ArianeAdaptor*>(sm.requestDeviceAdaptor(ADAPTOR_ID));
    QVERIFY ( ariane1 );

    ArianeAdaptor* ariane2 = dynamic_cast<ArianeAdaptor*>(sm.requestDeviceAdaptor(ADAPTOR_ID));
    QVERIFY ( ariane2 );

    QCOMPARE ( ariane1, ariane2 );

    sm.releaseDeviceAdaptor(ADAPTOR_ID);
    sm.releaseDeviceAdaptor(ADAPTOR_ID);
    //sm.releaseDeviceAdaptor(ADAPTOR_ID);
}

void AdaptorTest::testArianeDeviceAdaptorStartStop()
{
    ArianeAdaptor* adaptor = dynamic_cast<ArianeAdaptor*>(ArianeAdaptor::factoryMethod("arianeadaptor"));
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY( ok );

    const int INTERVAL = 500;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    adaptor->stopAdaptor();

    delete adaptor;
}

void AdaptorTest::testArianeDeviceAdaptorProperties()
{
    ArianeAdaptor* adaptor = dynamic_cast<ArianeAdaptor*>(ArianeAdaptor::factoryMethod("arianeadaptor"));
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY( ok );

    ArianeAdaptedSensorEntry* sensor = dynamic_cast<ArianeAdaptedSensorEntry*>(adaptor->findAdaptedSensor(QString::number(ORIENTATION_SENSOR_ID)));
    QVERIFY ( sensor );

    /*
    Sensor: "Orientation (Tom)" ( "2080" )
    */

    // test static properties
    //QVERIFY( sensor->buffer() ); // should be initialized
    QCOMPARE( sensor->name(), QString::number(ORIENTATION_SENSOR_ID) );
    QCOMPARE( sensor->description(), QString("Orientation (Tom)") );

    // TODO: find the right way to handle interval with respect to subsystem
    const unsigned INTERVAL_1 = 5000;
    sensor->setInterval( INTERVAL_1 );
    QCOMPARE( sensor->errorCode(), SaPropertyDoesNotExist );

    QVERIFY( !sensor->isRunning() );

    /*
    Sensor Properties:
    sensor id        = 2080
    sample rate [Hz] = 20
    resolution bits  = 16
    sample channels  = 3
    bit flags        = 2
    unit conversion of sample 0 : a = 9.18355e-41 , b = 0
    unit conversion of sample 1 : a = 9.18355e-41 , b = 0
    unit conversion of sample 2 : a = 9.18355e-41 , b = 0
    */
    QCOMPARE( sensor->hwSensorId(),      uint16_t(ORIENTATION_SENSOR_ID) );
    QCOMPARE( sensor->measurementType(), uint8_t(ARIANE_MTYPE_ORIENTATION) );
    //QCOMPARE( sensor->sensorLocation(),  uint8_t(HOST_LOCATION) );
    QCOMPARE( sensor->sensorNumber(),    uint8_t(0) );
    QCOMPARE( sensor->sampleRate(),      uint16_t(20) );
    QCOMPARE( sensor->resolutionBits(),  uint8_t(16) );
    QCOMPARE( sensor->sampleChannels(),  uint8_t(3) );
    QCOMPARE( sensor->flags(),           uint16_t(2) );

    // test dynamic properties

    /*
    Sensor Parameters:
    sensor id        = 2080
    description      = Yaw angle mode
    param id         = 0
    min value        = 0
    max value        = 1
    flags            = 2
    */

    const char* YAW_ANGLE_MODE = "Yaw angle mode";
    QVERIFY( sensor->paramMap_.contains(YAW_ANGLE_MODE) );

    ariane_sensor_parameter_t& paramDescription = sensor->paramMap_[ YAW_ANGLE_MODE ];
    QCOMPARE( paramDescription.description,  YAW_ANGLE_MODE );
    QCOMPARE( paramDescription.paramid,      uint16_t(0) );
    QCOMPARE( paramDescription.min,          uint32_t(0) );
    QCOMPARE( paramDescription.max,          uint32_t(1) );
    QCOMPARE( paramDescription.flags,        uint16_t(2) );

    //printParamDescription(paramDescription);

    QVariant currentValue = sensor->property ( YAW_ANGLE_MODE );

    unsigned writeValue = currentValue.toInt() == 0 ? 1 : 0;
    sensor->setProperty ( YAW_ANGLE_MODE, QVariant( writeValue ) );

    QVariant readValue = sensor->property ( YAW_ANGLE_MODE );
    QCOMPARE( readValue, QVariant( writeValue ) );

    adaptor->stopAdaptor();

    delete adaptor;
}

void AdaptorTest::testArianeDeviceAdaptor()
{
    TestArianeAdaptor* adaptor = new TestArianeAdaptor;
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY( ok );

    ok = adaptor->startSensor(QString::number(ORIENTATION_SENSOR_ID));
    QVERIFY( ok );

    const int INTERVAL = 1000;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    qDebug() << "handleTimedXyzDataCount_         =" << adaptor->handleTimedXyzDataCount_;
    QVERIFY( adaptor->handleTimedXyzDataCount_ > 0);

    adaptor->stopSensor(QString::number(ORIENTATION_SENSOR_ID));

    adaptor->stopAdaptor();

    delete adaptor;
}

void AdaptorTest::testArianeDeviceAdaptorVariantMode()
{
    TestArianeAdaptor* adaptor = new TestArianeAdaptor;
    QVERIFY( adaptor );

    QCOMPARE( adaptor->variantMode(), false );
    adaptor->setVariantMode( true );
    QCOMPARE( adaptor->variantMode(), true );

    bool ok = adaptor->startAdaptor();
    QVERIFY( ok );

#define USE_MAGNETOMETER_SENSOR
#ifdef USE_MAGNETOMETER_SENSOR
    ok = adaptor->startSensor(QString::number(MAGNETOMETER_SENSOR_ID));
#else
    ok = adaptor->startSensor(QString::number(THERMISTOR_SENSOR_ID));
#endif
    QVERIFY( ok );

    const int INTERVAL = 1000;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

#ifdef USE_MAGNETOMETER_SENSOR
    qDebug() << "handleTimedXyzDataVariantCount_  =" << adaptor->handleTimedXyzDataVariantCount_;
    QVERIFY( adaptor->handleTimedXyzDataVariantCount_ > 0);
#else
    qDebug() << "handleTimedUnsignedVariantCount_ =" << adaptor->handleTimedUnsignedVariantCount_;
    QVERIFY( adaptor->handleTimedUnsignedVariantCount_ > 0);
#endif

#ifdef USE_MAGNETOMETER_SENSOR
    adaptor->stopSensor(QString::number(MAGNETOMETER_SENSOR_ID));
#else
    adaptor->stopSensor(QString::number(THERMISTOR_SENSOR_ID));
#endif

    adaptor->stopAdaptor();

    delete adaptor;
}

void AdaptorTest::testArianeDeviceAdaptorIntervalProperty()
{
    TestArianeAdaptor* adaptor = new TestArianeAdaptor;
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY ( ok );

    // test an adapted sensor without sample rate parameter
    ArianeAdaptedSensorEntry* sysSensor = dynamic_cast<ArianeAdaptedSensorEntry*>(adaptor->findAdaptedSensor(QString::number(SYSTEM_SENSOR_ID)));
    QVERIFY( sysSensor );

    unsigned INTERVAL_ABSENT = 25;
    sysSensor->setInterval( INTERVAL_ABSENT );
    QCOMPARE( sysSensor->errorCode(), SaPropertyDoesNotExist );


    // test an adapted sensor with sample rate parameter
    ArianeAdaptedSensorEntry* magSensor = dynamic_cast<ArianeAdaptedSensorEntry*>(adaptor->findAdaptedSensor(QString::number(MAGNETOMETER_SENSOR_ID)));
    QVERIFY( magSensor );

    // out of range (10 <= rate <= 20 -> 50 <= interval <= 100)
    unsigned INTERVAL_TOO_LOW = 25;
    magSensor->setInterval( INTERVAL_TOO_LOW );
    QCOMPARE( magSensor->errorCode(), SaValueOutOfRange );

    unsigned INTERVAL_TOO_HIGH = 150;
    magSensor->setInterval( INTERVAL_TOO_HIGH );
    QCOMPARE( magSensor->errorCode(), SaValueOutOfRange );

    // in range (10 <= rate <= 20 -> 50 <= interval <= 100)
    const unsigned ERROR_MARGIN = 5;
    unsigned INTERVAL_VALID = 80;
    magSensor->setInterval( INTERVAL_VALID );
    QCOMPARE( magSensor->errorCode(), SNoError );
    unsigned actualInterval = magSensor->interval();
    QVERIFY (  actualInterval < (INTERVAL_VALID + ERROR_MARGIN) );
    QVERIFY (  actualInterval > (INTERVAL_VALID - ERROR_MARGIN) );
    QCOMPARE ( actualInterval, (unsigned)magSensor->property("interval").toInt() );

    adaptor->stopAdaptor();

    delete adaptor;
}

void AdaptorTest::testArianeDeviceAdaptorMultipleVariant()
{
    ArianeAdaptor* adaptor = dynamic_cast<ArianeAdaptor*>(ArianeAdaptor::factoryMethod("arianeadaptor"));
    QVERIFY( adaptor );

    adaptor->setVariantMode( true );
    QCOMPARE( adaptor->variantMode(), true );

    bool ok = adaptor->startAdaptor();
    QVERIFY( ok );


    QString magSensorName = QString::number(MAGNETOMETER_SENSOR_ID);

    qDebug() << "Preparing magnetometer";
    ArianeAdaptedSensorEntry* magSensor = dynamic_cast<ArianeAdaptedSensorEntry*>(adaptor->findAdaptedSensor(magSensorName));
    QVERIFY( magSensor );

    const int MAG_INTERVAL = 1000/20;   // 20 samples per second
    magSensor->setInterval(MAG_INTERVAL); // ms
    ok = adaptor->startSensor(magSensorName);
    QVERIFY( ok );


    qDebug() << "Preparing accelerometer";
    QString accSensorName = QString::number(ACCELEROMETER_2G_SENSOR_ID);

    ArianeAdaptedSensorEntry* accSensor = dynamic_cast<ArianeAdaptedSensorEntry*>(adaptor->findAdaptedSensor(accSensorName));
    QVERIFY( accSensor );

    const int ACC_INTERVAL = 1000/20;   // 20 samples per second
    magSensor->setInterval(ACC_INTERVAL); // ms
    ok = adaptor->startSensor(accSensorName);
    QVERIFY( ok );


    QString gyrSensorName = QString::number(GYROSCOPE_SENSOR_ID);

    qDebug() << "Preparing gyrometer";
    ArianeAdaptedSensorEntry* gyrSensor = dynamic_cast<ArianeAdaptedSensorEntry*>(adaptor->findAdaptedSensor(gyrSensorName));
    QVERIFY( gyrSensor );

    const int GYR_INTERVAL = 1000/100;   // 100 samples per second
    gyrSensor->setInterval(GYR_INTERVAL); // ms
    ok = adaptor->startSensor(gyrSensorName);
    QVERIFY( ok );


    const int DURATION = 1000;
    qDebug ( "Waiting %d mseconds", DURATION );
    QTest::qWait( DURATION );

    //QVERIFY( test1.numSamplesReceived > 0 );
    //QVERIFY( test2.numSamplesReceived > 0 );
    //QVERIFY( test3.numSamplesReceived > 0 );

    adaptor->stopSensor(magSensorName);

    adaptor->stopSensor(accSensorName);

    adaptor->stopSensor(gyrSensorName);

    adaptor->stopAdaptor();

    delete adaptor;
}

#endif

#ifdef USE_INTERNAL
void AdaptorTest::testAccelerometerAdaptor()
{
    AccelerometerAdaptor* adaptor = dynamic_cast<AccelerometerAdaptor*>(AccelerometerAdaptor::factoryMethod("accelerometeradaptor"));
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY(ok);

    const int INTERVAL    = 500;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    // TODO: some checking of output might be nice.

    adaptor->stopAdaptor();
}

void AdaptorTest::testMagnetometerAdaptor()
{
    MagnetometerAdaptor* adaptor = dynamic_cast<MagnetometerAdaptor*>(MagnetometerAdaptor::factoryMethod("magnetometeradaptor"));
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY(ok);

    const int INTERVAL    = 500;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    // TODO: some checking of output might be nice.

    adaptor->stopAdaptor();
}

void AdaptorTest::testALSAdaptor()
{
    ALSAdaptor* adaptor = dynamic_cast<ALSAdaptor*>(ALSAdaptor::factoryMethod("alsadaptor"));
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY(ok);

    const int INTERVAL    = 500;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    // TODO: some checking of output might be nice.

    adaptor->stopAdaptor();
}

/**
 * Can not test output here as it would require physical event to occur on device.
 */
void AdaptorTest::testTapAdaptor()
{
    TapAdaptor* adaptor = dynamic_cast<TapAdaptor*>(TapAdaptor::factoryMethod("tapadaptor"));
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY(ok);

    const int INTERVAL = 500;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    adaptor->stopAdaptor();
}

/**
 * Can not test output here as it would require physical event to occur on device.
 */
void AdaptorTest::testTouchAdaptor()
{
    TouchAdaptor* adaptor = dynamic_cast<TouchAdaptor*>(TouchAdaptor::factoryMethod("touchadaptor"));
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY( ok );

    const int INTERVAL = 500;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    adaptor->stopAdaptor();
}

/**
 * Keyboard slider state.
 */
void AdaptorTest::testKeyboardSliderAdaptor()
{
    KeyboardSliderAdaptor* adaptor = dynamic_cast<KeyboardSliderAdaptor*>(KeyboardSliderAdaptor::factoryMethod("kbslideradaptor"));
    QVERIFY( adaptor );

    bool ok = adaptor->startAdaptor();
    QVERIFY( ok );

    const int INTERVAL = 500;
    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    adaptor->stopAdaptor();    
}



void AdaptorTest::testProximityAdaptor()
{
    ProximityAdaptor* adaptor = dynamic_cast<ProximityAdaptor*>(ProximityAdaptor::factoryMethod("proximityadaptor"));
    QVERIFY( adaptor );
    bool ok = adaptor->startAdaptor();
    QVERIFY(ok);

    const int INTERVAL = 500;
    qDebug ("Waiting %d mseconds", INTERVAL);
    QTest::qWait(INTERVAL);

    adaptor->stopAdaptor();
}

#endif

QTEST_MAIN(AdaptorTest)
