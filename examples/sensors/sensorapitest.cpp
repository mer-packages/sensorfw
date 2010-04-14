#include <QDBusMetaType>

#ifdef USE_MOTION_BAND
#include "raworientationsensor.h"
#endif

#ifdef USE_INTERNAL
#include "orientationsensor.h"
#include "accelerometersensor.h"
#include "proximitysensor.h"
#ifdef USE_COMPASS
#include "compasssensor.h"
#endif
#include "alssensor.h"
#endif
#ifdef USE_ARIANE
#include "filteredsensor.h"
#include "arianesensor.h"
#include "directsensormanager.h"
#endif
#include "testclass.h"

#include "sensorapitest.h"

QString SensorApiTest::bluetoothAddress1 = "00:02:5B:00:A6:DA";
QString SensorApiTest::bluetoothAddress2 = "00:02:5B:00:A6:3F";

void SensorApiTest::initTestCase()
{
    // called before everything else (in this case to register types)
#ifdef USE_MOTION_BAND
    qDBusRegisterMetaType<RawOrientation>();
#endif
    qDBusRegisterMetaType<Orientation>();
    qDBusRegisterMetaType<XYZ>();
    qDBusRegisterMetaType<Unsigned>();
#ifdef USE_ARIANE
    qDBusRegisterMetaType<Key>();
#endif

    SensorManager& sm = SensorManager::instance();

#ifdef USE_MOTION_BAND
    qDebug() << sm.loadPlugin("raworientationsensor");
#endif

#ifdef USE_ARIANE
    qDebug() << sm.loadPlugin("arianesensor");
    qDebug() << sm.loadPlugin("filteredsensor");
#endif

#ifdef USE_COMPASS    
    qDebug() << sm.loadPlugin("compasssensor");
#endif

#ifdef USE_INTERNAL
    qDebug() << sm.loadPlugin("libaccelerometersensor");
    qDebug() << sm.loadPlugin("orientationsensor");
    qDebug() << sm.loadPlugin("tapsensor");
    qDebug() << sm.loadPlugin("alssensor");
    qDebug() << sm.loadPlugin("proximitysensor");
#endif

    // TODO: Segfaults.
    //sm.print();
}

#ifdef USE_ARIANE

void SensorApiTest::testDeviceAdaptorRegistration()
{
    SensorManager& sm = SensorManager::instance();

    QString adaptorName      = "testarianeadaptor";
    QString adaptorParams    = ";bluetoothAddress=" + bluetoothAddress1;

    sm.registerDeviceAdaptor<ArianeAdaptor>(adaptorName + adaptorParams);

    ArianeAdaptor* adaptor = dynamic_cast<ArianeAdaptor*>(sm.requestDeviceAdaptor(adaptorName));
    QVERIFY( adaptor );

    QCOMPARE( adaptor->bluetoothAddress(), bluetoothAddress1);
    QCOMPARE( adaptor->property("bluetoothAddress").toString(), bluetoothAddress1);

    sm.releaseDeviceAdaptor(adaptorName);
}

void SensorApiTest::testDeviceAdaptorManagement()
{
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<ArianeAdaptor>("arianeadaptor");

    TestErrorSignalReceiver errorReceiver;
    //bool ok = connect(&sm, SIGNAL(errorCode(SensorManagerError)), &errorReceiver, SLOT(errorSlot(SensorManagerError)));
    bool ok = connect(&sm, SIGNAL(errorSignal(int)), &errorReceiver, SLOT(errorSlot(int)));
    QVERIFY ( ok );

    int numErrorSignals = 0;

    // case: request an unknown device adaptor (fails)
    DeviceAdaptor* adaptor = sm.requestDeviceAdaptor("unknownadaptor");
    QVERIFY( adaptor == NULL );
    QCOMPARE( sm.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );

    // case: release an unknown device adaptor (fails)
    sm.releaseDeviceAdaptor("unknownadaptor");
    QCOMPARE( sm.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );

    // case: request an existing device adaptor (succeeds)
    adaptor = sm.requestDeviceAdaptor("arianeadaptor");
    QVERIFY( adaptor );
    QCOMPARE( sm.errorCode(), SmNoError );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );

    // case: release an instantiated device adaptor (succeeds)
    sm.releaseDeviceAdaptor("arianeadaptor");
    QCOMPARE( sm.errorCode(), SmNoError );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );

    // case: release an already released device adaptor (fails)
    sm.releaseDeviceAdaptor("arianeadaptor");
    QCOMPARE( sm.errorCode(), SmNotInstantiated );
    QCOMPARE( errorReceiver.errorCode(), SmNotInstantiated );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );
}

void SensorApiTest::testSensorManagement()
{
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<ArianeAdaptor>("arianeadaptor");
    sm.registerSensor<ArianeSensorChannel>("arianesensor");

    TestErrorSignalReceiver errorReceiver;
    //bool ok = connect(&sm, SIGNAL(error(SensorManagerError)), &errorReceiver, SLOT(errorSlot(SensorManagerError)));
    bool ok = connect(&sm, SIGNAL(errorSignal(int)), &errorReceiver, SLOT(errorSlot(int)));
    QVERIFY ( ok );

    int numErrorSignals = 0;

    // case: request an unknown control sensor (fails)
    int controlSessionId1 = sm.requestControlSensor("unknownsensor;a=1,b=2,c=3");
    QCOMPARE( controlSessionId1, INVALID_SESSION);
    QCOMPARE( sm.errorCode(), SmIdNotRegistered );
    QVERIFY(  !sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );

    // case: request an existing listen sensor that is not controlled (fails)
    int listenSessionId1 = sm.requestListenSensor("arianesensor;a=1,b=2,c=3");
    QCOMPARE( listenSessionId1, INVALID_SESSION);
    QCOMPARE( sm.errorCode(), SmNotInstantiated );
    QVERIFY(  !sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.errorCode(), SmNotInstantiated );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );

    // case: request an existing control sensor (succeeds)
    controlSessionId1 = sm.requestControlSensor("arianesensor;adaptor=arianeadaptor,a=1,b=2,c=3");
    QVERIFY( controlSessionId1 >= 0);
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );

    // case: request a second existing control sensor (fails)
    int controlSessionId2 = sm.requestControlSensor("arianesensor;adaptor=arianeadaptor,a=1,b=2,c=3");
    QCOMPARE( controlSessionId2, INVALID_SESSION);
    QCOMPARE( sm.errorCode(), SmAlreadyUnderControl );
    QVERIFY(  !sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.errorCode(), SmAlreadyUnderControl );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );

    // case: request a second existing listen sensor that is controlled (succeeds)
    listenSessionId1 = sm.requestListenSensor("arianesensor;adaptor=arianeadaptor,a=1,b=2,c=3");
    QVERIFY( listenSessionId1 >= 0);
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );

    // case: release existing control sensor, but wrong session ID (fails)
    ok = sm.releaseSensor("unknownsensor", controlSessionId1);
    QVERIFY( !ok );
    QCOMPARE( sm.errorCode(), SmIdNotRegistered );
    QVERIFY(  !sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );

    // case: release existing control sensor, with correct session ID (succeeds)
    ok = sm.releaseSensor("arianesensor", controlSessionId1);
    QVERIFY( ok );
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );

    // case: release second non-existing control sensor (fails)
    ok = sm.releaseSensor("arianesensor", controlSessionId2);
    QVERIFY( !ok );
    QCOMPARE( sm.errorCode(), SmNotInstantiated );
    QVERIFY(  !sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.errorCode(), SmNotInstantiated );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );

    // case: request a second existing listen sensor (succeeds)
    int listenSessionId2 = sm.requestListenSensor("arianesensor;adaptor=arianeadaptor,a=1,b=2,c=3");
    QVERIFY(listenSessionId2 >= 0);
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );

    // case: request a non-existing listen sensor (fails)
    int listenSessionId3 = sm.requestListenSensor("unknownsensor;adaptor=arianeadaptor,a=1,b=2,c=3");
    QCOMPARE( listenSessionId3, INVALID_SESSION);
    QCOMPARE( sm.errorCode(), SmIdNotRegistered );
    QVERIFY(  !sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );

    // case: release first existing listen sensor (succeeds)
    ok = sm.releaseSensor("arianesensor", listenSessionId1);
    QVERIFY( ok );
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );

    // case: request a (new) first existing control sensor (succeeds)
    int controlSessionId3 = sm.requestControlSensor("arianesensor;adaptor=arianeadaptor,a=4,b=5,c=6");
    QVERIFY(controlSessionId3 >= 0);
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );

    // case: release second existing listen sensor (succeeds)
    ok = sm.releaseSensor("arianesensor", listenSessionId2);
    QVERIFY( ok );
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );

    // case: release third already released listen sensor (fails)
    ok = sm.releaseSensor("arianesensor", listenSessionId2);
    QVERIFY( !ok );
    QCOMPARE( sm.errorCode(), SmNotInstantiated );
    QVERIFY(  !sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.errorCode(), SmNotInstantiated );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );

    // case: release third non-existing listen sensor (fails)
    ok = sm.releaseSensor("unknownsensor", listenSessionId3);
    QVERIFY( !ok );
    QCOMPARE( sm.errorCode(), SmIdNotRegistered );
    QVERIFY(  !sm.errorString().isEmpty() );
    QCOMPARE( errorReceiver.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );

    // case: release third non-existing listen sensor (fails)
    ok = sm.releaseSensor("arianesensor", controlSessionId3);
    QVERIFY( ok );
    QVERIFY(  sm.errorString().isEmpty() );
    QCOMPARE( sm.errorCode(), SmNoError );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );
}

#endif

#ifdef USE_MOTION_BAND
void SensorApiTest::testRawOrientationSensor()
{
    QString RAW_ORIENTATION_SENSOR_ID = "raworientationsensor";
    SensorHandle<RawOrientationSensorChannel> sensor = DirectSensorFactory<RawOrientationSensorChannel>::controlSensor(RAW_ORIENTATION_SENSOR_ID);
    QVERIFY ( sensor );

    TestClass test("testRawOrientationSensor");

    QCOMPARE( sensor->id(), QString("raworientationsensor") );

    const int INTERVAL    = 1000;
    const int NUM_SIGNALS = 5;

    sensor->setInterval(INTERVAL); // ms
    QCOMPARE( sensor->interval(), INTERVAL );
    QCOMPARE( sensor->property("interval").toInt(), INTERVAL );

    connect(sensor, SIGNAL(dataAvailable(const RawOrientation&)),
            &test, SLOT(handleData(const RawOrientation&)));

    sensor->start();

    qDebug ( "Waiting %d mseconds", NUM_SIGNALS * INTERVAL );
    QTest::qWait( NUM_SIGNALS * INTERVAL + INTERVAL / 2 );

    QCOMPARE( test.numSamplesReceived, NUM_SIGNALS );

    sensor->stop();

    DirectSensorFactory<RawOrientationSensorChannel>::releaseSensor(sensor);
    delete sensor;
}
#endif

#ifdef USE_ARIANE
void SensorApiTest::testSensorReconnectionAndChange()
{

    qDebug() << "Switch sensor box off...";
    QTest::qWait( 5000 );
    qDebug() << "...starting.";

    SensorManager& sm = SensorManager::instance();

    QString adaptorName      = "testarianeadaptor";
    QString adaptorParams    = ";bluetoothAddress=" + bluetoothAddress1;

    sm.registerDeviceAdaptor<ArianeAdaptor>(adaptorName + adaptorParams);

    sm.registerSensor<ArianeSensorChannel>("arianesensor");
    const QString DEVICE_SENSOR_ID = "arianesensor;hardwareSensorId=2064,adaptor=testarianeadaptor";
    ArianeSensorChannel* deviceSensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(DEVICE_SENSOR_ID);
    QVERIFY ( !deviceSensor );

    qDebug() << "Switch sensor box on...";
    QTest::qWait( 5000 );
    qDebug() << "...retrying.";

    deviceSensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(DEVICE_SENSOR_ID);
    QVERIFY ( deviceSensor && deviceSensor->isValid() );
}

void SensorApiTest::testFilteredSensor()
{
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<ArianeAdaptor>("arianeadaptor");
    sm.registerSensor<FilteredSensorChannel>("filteredsensor");

    QString FILTERED_SENSOR_ID = "filteredsensor;adaptor=arianeadaptor";
    FilteredSensorChannel* sensor = DirectSensorFactory<FilteredSensorChannel>::controlSensor(FILTERED_SENSOR_ID);
    QVERIFY ( sensor );

    TestClass test("testFilteredSensor");

    QCOMPARE( sensor->id(), getCleanId(FILTERED_SENSOR_ID) );

    const int DURATION = 1000;

    connect(sensor, SIGNAL(dataAvailable(const Orientation&)),
            &test, SLOT(handleData(const Orientation&)));

    sensor->start();
    QCOMPARE (sensor->errorCode(), SNoError );

    qDebug ( "Waiting %d mseconds", DURATION );
    QTest::qWait( DURATION );

    qDebug() << "Number of received samples:" << test.numSamplesReceived;
    QVERIFY( test.numSamplesReceived > 0 );

    sensor->stop();
    QCOMPARE (sensor->errorCode(), SNoError );

    DirectSensorFactory<FilteredSensorChannel>::releaseSensor(sensor);
}

void SensorApiTest::testArianeSensorInterval()
{
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<ArianeAdaptor>("arianeadaptor");
    sm.registerSensor<ArianeSensorChannel>("arianesensor");

    const QString ARIANE_SENSOR_ID = "arianesensor;adaptor=arianeadaptor";
    ArianeSensorChannel* sensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(ARIANE_SENSOR_ID);
    QVERIFY ( sensor );

    const int LOCATION = ARIANE_DEVICE_LOCATION_WRIST;
    const int SENSOR_ID = ARIANE_SID(0, LOCATION, ARIANE_MTYPE_MAGNETIC_FIELD);

    sensor->setHardwareSensorId(SENSOR_ID);
    QCOMPARE( sensor->hardwareSensorId(), SENSOR_ID );


    // out of range (10 <= rate <= 20 -> 50 <= interval <= 100)
    int INTERVAL_TOO_LOW = 25;
    sensor->setInterval( INTERVAL_TOO_LOW );
    QCOMPARE( sensor->errorCode(), SaValueOutOfRange );

    int INTERVAL_TOO_HIGH = 150;
    sensor->setInterval( INTERVAL_TOO_HIGH );
    QCOMPARE( sensor->errorCode(), SaValueOutOfRange );

    // in range (10 <= rate <= 20 -> 50 <= interval <= 100)
    const int ERROR_MARGIN = 5;
    int INTERVAL_VALID = 80;
    sensor->setInterval( INTERVAL_VALID );
    QCOMPARE( sensor->errorCode(), SNoError );
    QVERIFY ( sensor->interval() < (INTERVAL_VALID + ERROR_MARGIN) );
    QVERIFY ( sensor->interval() > (INTERVAL_VALID - ERROR_MARGIN) );

    DirectSensorFactory<ArianeSensorChannel>::releaseSensor(sensor);
}

void SensorApiTest::testArianeSensor()
{
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<ArianeAdaptor>("arianeadaptor");
    sm.registerSensor<ArianeSensorChannel>("arianesensor");

    const QString ARIANE_SENSOR_ID = "arianesensor;adaptor=arianeadaptor";
    ArianeSensorChannel* sensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(ARIANE_SENSOR_ID);
    QVERIFY ( sensor );

    TestClass test("testarianesensor");

    QCOMPARE( sensor->id(), QString("arianesensor") );

    const int INTERVAL    = 20;
    const int NUM_SIGNALS = 50;


    const int LOCATION = ARIANE_DEVICE_LOCATION_WRIST;
    const int SENSOR_ID = ARIANE_SID(0, LOCATION, ARIANE_MTYPE_ACCELERATION);

    sensor->setHardwareSensorId(SENSOR_ID);
    QCOMPARE( sensor->hardwareSensorId(), SENSOR_ID );
    QCOMPARE( sensor->property("hardwareSensorId").toInt(), SENSOR_ID );

    sensor->setInterval(INTERVAL); // ms
    QCOMPARE( sensor->interval(), INTERVAL );
    QCOMPARE( sensor->property("interval").toInt(), INTERVAL );

    bool ok = connect(sensor, SIGNAL(dataAvailable(const XYZ&)),
                      &test, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );


    sensor->start();
    QCOMPARE (sensor->errorCode(), SNoError );

    qDebug ( "Waiting %d mseconds", NUM_SIGNALS * INTERVAL );
    QTest::qWait( NUM_SIGNALS * INTERVAL + INTERVAL / 2 );

    qDebug() << "Number of received samples:" << test.numSamplesReceived;
    QVERIFY( test.numSamplesReceived > 0 );

    sensor->stop();
    QCOMPARE (sensor->errorCode(), SNoError );

    DirectSensorFactory<ArianeSensorChannel>::releaseSensor(sensor);
}

void SensorApiTest::testArianeTapSensor()
{
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<ArianeAdaptor>("arianeadaptor");
    sm.registerSensor<ArianeSensorChannel>("arianesensor");

    const QString TAP_ARIANE_SENSOR_ID = "arianesensor;adaptor=arianeadaptor,hardwareSensorId=1809";
    ArianeSensorChannel* sensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(TAP_ARIANE_SENSOR_ID);
    QVERIFY ( sensor );

    /*
    param id	 = 2
    description	 = ' Bit flag options '
    min value	 = 0
    max value	 = 255
    flags	 = 2 ( PARAM_FLAG_READONLY = false , PARAM_FLAG_SIGNED = false  )
    value (cached)   = 3
    */
    const QString  PROP_NAME_BIT_FLAG_OPTIONS  = "Bit flag options";
    const uint32_t PROP_VALUE_BIT_FLAG_OPTIONS = 3;
    sensor->setAdaptorProperty( PROP_NAME_BIT_FLAG_OPTIONS, PROP_VALUE_BIT_FLAG_OPTIONS );
    QCOMPARE( sensor->adaptorProperty( PROP_NAME_BIT_FLAG_OPTIONS), PROP_VALUE_BIT_FLAG_OPTIONS );

    /*
    param id	 = 3
    description	 = ' Deactivation timeout (ms) '
    min value	 = 500
    max value	 = 4294967295
    flags	 = 2 ( PARAM_FLAG_READONLY = false , PARAM_FLAG_SIGNED = false  )
    value (cached)   = 2000
    */
    const QString  PROP_NAME_DEACT_TIMEOUT  = "Deactivation timeout (ms)";
    const uint32_t PROP_VALUE_DEACT_TIMEOUT = 3000;
    sensor->setAdaptorProperty( PROP_NAME_DEACT_TIMEOUT, PROP_VALUE_DEACT_TIMEOUT );
    QCOMPARE( sensor->adaptorProperty( PROP_NAME_DEACT_TIMEOUT ), PROP_VALUE_DEACT_TIMEOUT );

    /*
    param id	 = 1
    description	 = ' Filter bits '
    min value	 = 1
    max value	 = 10
    flags	 = 2 ( PARAM_FLAG_READONLY = false , PARAM_FLAG_SIGNED = false  )
    value (cached)   = 6
    */
    const QString  PROP_NAME_FILTER_BITS  = "Filter bits";
    const uint32_t PROP_VALUE_FILTER_BITS = 6;
    sensor->setAdaptorProperty( PROP_NAME_FILTER_BITS, PROP_VALUE_FILTER_BITS );
    QCOMPARE( sensor->adaptorProperty( PROP_NAME_FILTER_BITS ), PROP_VALUE_FILTER_BITS );

    /*
    param id	 = 0
    description	 = ' Tap threshold '
    min value	 = 0
    max value	 = 32767
    flags	 = 2 ( PARAM_FLAG_READONLY = false , PARAM_FLAG_SIGNED = false  )
    value (cached)   = 20000
    */
    const QString  PROP_NAME_TAP_THRESHOLD  = "Tap threshold";
    const uint32_t PROP_VALUE_TAP_THRESHOLD = 25000;
    sensor->setAdaptorProperty( PROP_NAME_TAP_THRESHOLD, PROP_VALUE_TAP_THRESHOLD );
    QCOMPARE( sensor->adaptorProperty( PROP_NAME_TAP_THRESHOLD ), PROP_VALUE_TAP_THRESHOLD );

    TestClass test("testVariantTapSensor");

    QCOMPARE( sensor->id(), QString("arianesensor") );

    bool ok = connect(sensor, SIGNAL(dataAvailable(const Key&)),
                      &test, SLOT(handleData(const Key&)));
    QVERIFY( ok );


    sensor->start();
    QCOMPARE (sensor->errorCode(), SNoError );

    const int WAITING_DELAY = 10000;
    qDebug ( "Waiting %d mseconds", WAITING_DELAY );
    QTest::qWait( WAITING_DELAY );

    qDebug() << "Number of received samples:" << test.numSamplesReceived;
    //QVERIFY( test.numSamplesReceived > 0 );


    sensor->stop();
    QCOMPARE (sensor->errorCode(), SNoError );

    DirectSensorFactory<ArianeSensorChannel>::releaseSensor(sensor);
}

void SensorApiTest::testMultipleArianeSensor()
{
    const int LOCATION = ARIANE_DEVICE_LOCATION_WRIST;

    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<ArianeAdaptor>("arianeadaptor");

    TestClass magTest("MagneticField");

    sm.registerSensor<ArianeSensorChannel>("arianesensor");
    const QString ARIANE_SENSOR_ID = "arianesensor;adaptor=arianeadaptor";
    ArianeSensorChannel* magSensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(ARIANE_SENSOR_ID);
    QVERIFY ( magSensor && magSensor->isValid() );

    const int MAG_INTERVAL = 1000/10;
    magSensor->setHardwareSensorId(ARIANE_SID(0, LOCATION, ARIANE_MTYPE_MAGNETIC_FIELD));
    magSensor->setInterval(MAG_INTERVAL); // ms
    qDebug() << "MagSensor interval =" << magSensor->interval();
    bool ok = connect(magSensor, SIGNAL(dataAvailable(const XYZ&)),
                      &magTest, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );
    magSensor->start();
    QCOMPARE (magSensor->errorCode(), SNoError );

    TestClass accTest("Accelerometer");

    sm.registerSensor<ArianeSensorChannel>("arianesensor2");
    const QString ARIANE_SENSOR_ID2 = "arianesensor2;adaptor=arianeadaptor";
    ArianeSensorChannel* accSensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(ARIANE_SENSOR_ID2);
    QVERIFY ( accSensor && accSensor->isValid() );

    accSensor->setHardwareSensorId(ARIANE_SID(0, LOCATION, ARIANE_MTYPE_ACCELERATION));
    const int ACC_INTERVAL = 1000/80;
    accSensor->setInterval(ACC_INTERVAL); // ms
    qDebug() << "AccSensor interval =" << accSensor->interval();
    ok = connect(accSensor, SIGNAL(dataAvailable(const XYZ&)),
                 &accTest, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );
    accSensor->start();
    QCOMPARE ( accSensor->errorCode(), SNoError );


    TestClass gyrTest("AngularRate");

    sm.registerSensor<ArianeSensorChannel>("arianesensor3");
    const QString ARIANE_SENSOR_ID3 = "arianesensor3;adaptor=arianeadaptor";
    ArianeSensorChannel* gyrSensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(ARIANE_SENSOR_ID3);
    QVERIFY ( gyrSensor && gyrSensor->isValid() );

    gyrSensor->setHardwareSensorId(ARIANE_SID(0, LOCATION, ARIANE_MTYPE_ANGULAR_RATE));
    const int GYR_INTERVAL = 1000/40;
    gyrSensor->setInterval(GYR_INTERVAL); // ms
    qDebug() << "GyrSensor interval =" << gyrSensor->interval();
    ok = connect(gyrSensor, SIGNAL(dataAvailable(const XYZ&)),
                 &gyrTest, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );
    gyrSensor->start();
    QCOMPARE ( gyrSensor->errorCode(), SNoError );


    const int WAIT_DELAY = 500;
    qDebug ( "Waiting %d mseconds", WAIT_DELAY );
    QTest::qWait( WAIT_DELAY );

    qDebug() << "Samples received by magTest" << magTest.numSamplesReceived;
    qDebug() << "Samples received by accTest" << accTest.numSamplesReceived;
    qDebug() << "Samples received by gyrTest" << gyrTest.numSamplesReceived;

    QVERIFY( magTest.numSamplesReceived > 0 );
    QVERIFY( accTest.numSamplesReceived > 0 );
    QVERIFY( gyrTest.numSamplesReceived > 0 );


    magSensor->stop();
    QCOMPARE ( magSensor->errorCode(), SNoError );

    DirectSensorFactory<ArianeSensorChannel>::releaseSensor(magSensor);

    accSensor->stop();
    QCOMPARE ( accSensor->errorCode(), SNoError );

    DirectSensorFactory<ArianeSensorChannel>::releaseSensor(accSensor);

    gyrSensor->stop();
    QCOMPARE ( gyrSensor->errorCode(), SNoError );

    DirectSensorFactory<ArianeSensorChannel>::releaseSensor(gyrSensor);
}


void SensorApiTest::testArianeTwoSensorBoxes()
{
    SensorManager& sm = SensorManager::instance();

    qDebug() << "Creating TAP_ARIANE_SENSOR_ID_1";
    sm.registerDeviceAdaptor<ArianeAdaptor>("arianeadaptor");
    sm.registerSensor<ArianeSensorChannel>("arianesensor");
    const QString TAP_ARIANE_SENSOR_ID_1 = "arianesensor;hardwareSensorId=2064,adaptor=arianeadaptor";
    ArianeSensorChannel* sensor1 = DirectSensorFactory<ArianeSensorChannel>::controlSensor(TAP_ARIANE_SENSOR_ID_1);
    QVERIFY ( sensor1 && sensor1->isValid() );

    QCOMPARE( sensor1->id(), QString("arianesensor") );

    qDebug() << "Creating TAP_ARIANE_SENSOR_ID_2";
    sm.registerDeviceAdaptor<ArianeAdaptor>("arianeadaptor2;bluetoothAddress=" + bluetoothAddress1);
    sm.registerSensor<ArianeSensorChannel>("arianesensor2");
    const QString TAP_ARIANE_SENSOR_ID_2 = "arianesensor2;hardwareSensorId=2064,adaptor=arianeadaptor2";
    ArianeSensorChannel* sensor2 = DirectSensorFactory<ArianeSensorChannel>::controlSensor(TAP_ARIANE_SENSOR_ID_2);
    QVERIFY ( sensor2 && sensor2->isValid() );

    QCOMPARE( sensor2->id(), QString("arianesensor2") );

    TestClass test1("Sensor1");
    bool ok = connect(sensor1, SIGNAL(dataAvailable(const XYZ&)),
                      &test1, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );
    sensor1->start();
    QCOMPARE ( sensor1->errorCode(), SNoError );

    TestClass test2("Sensor2");
    ok = connect(sensor2, SIGNAL(dataAvailable(const XYZ&)),
                 &test2, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );
    sensor2->start();
    QCOMPARE ( sensor2->errorCode(), SNoError );


    const int WAIT_DELAY = 5000;
    qDebug ( "Waiting %d mseconds", WAIT_DELAY );
    QTest::qWait( WAIT_DELAY );

    qDebug() << "Samples received by sensor 1" << test1.numSamplesReceived;
    qDebug() << "Samples received by sensor 2" << test2.numSamplesReceived;

    QVERIFY( test1.numSamplesReceived > 0 );
    QVERIFY( test2.numSamplesReceived > 0 );

    sensor1->stop();
    QCOMPARE ( sensor1->errorCode(), SNoError );

    DirectSensorFactory<ArianeSensorChannel>::releaseSensor(sensor1);

    sensor2->stop();
    QCOMPARE ( sensor2->errorCode(), SNoError );

    DirectSensorFactory<ArianeSensorChannel>::releaseSensor(sensor2);
}

void SensorApiTest::testSuomenlinna()
{
    SensorManager& sm = SensorManager::instance();

    qDebug() << "Preparing device sensor...";
    QString btAdaptor = "deviceadaptor";
    sm.registerDeviceAdaptor<ArianeAdaptor>(btAdaptor);
    sm.registerSensor<ArianeSensorChannel>("devicesensor");
    QString DEVICE_SENSOR_ID = "devicesensor;hardwareSensorId=2064,adaptor=deviceadaptor";
    ArianeSensorChannel* deviceSensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(DEVICE_SENSOR_ID);
    QVERIFY ( deviceSensor );


    qDebug() << "Preparing head sensor...";
    btAdaptor = "headadaptor;bluetoothAddress=" + bluetoothAddress1;
    sm.registerDeviceAdaptor<ArianeAdaptor>(btAdaptor);
    sm.registerSensor<ArianeSensorChannel>("headsensor");
    QString HEAD_SENSOR_ID = "headsensor;hardwareSensorId=2064,adaptor=headadaptor";
    ArianeSensorChannel* headSensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(HEAD_SENSOR_ID);
    QVERIFY ( headSensor );


    qDebug() << "Preparing head sensor...";
    btAdaptor = "wristadaptor;bluetoothAddress=" + bluetoothAddress2;
    sm.registerDeviceAdaptor<ArianeAdaptor>(btAdaptor);
    sm.registerSensor<ArianeSensorChannel>("wristsensor");
    QString WRIST_SENSOR_ID = "wristsensor;hardwareSensorId=2064,adaptor=wristadaptor";
    ArianeSensorChannel* wristSensor = DirectSensorFactory<ArianeSensorChannel>::controlSensor(WRIST_SENSOR_ID);
    QVERIFY ( wristSensor );


    qDebug() << "Starting device sensor...";
    deviceSensor->start();
    QCOMPARE (deviceSensor->errorCode(), SNoError );

    qDebug() << "Starting head sensor...";
    headSensor->start();
    QCOMPARE (headSensor->errorCode(), SNoError );

    qDebug() << "Starting wrist sensor...";
    wristSensor->start();
    QCOMPARE (wristSensor->errorCode(), SNoError );


    const int DURATION = 5000;
    qDebug ( "Waiting %d mseconds", DURATION );
    QTest::qWait( DURATION );

    qDebug() << "Stoping device sensor...";
    deviceSensor->stop();
    QCOMPARE ( deviceSensor->errorCode(), SNoError );
    DirectSensorFactory<ArianeSensorChannel>::releaseSensor( deviceSensor );

    qDebug() << "Stoping head sensor...";
    headSensor->stop();
    QCOMPARE ( headSensor->errorCode(), SNoError );
    DirectSensorFactory<ArianeSensorChannel>::releaseSensor( headSensor );

    qDebug() << "Stoping wrist sensor...";
    wristSensor->stop();
    QCOMPARE ( wristSensor->errorCode(), SNoError );
    DirectSensorFactory<ArianeSensorChannel>::releaseSensor( wristSensor );
}
#endif

#ifdef USE_INTERNAL
void SensorApiTest::testOrientationSensor()
{
    OrientationSensorChannel* sensor = dynamic_cast<OrientationSensorChannel*>(OrientationSensorChannel::factoryMethod("orientationsensor"));
    QVERIFY ( sensor );

    TestClass test("testOrientationSensor");

    QCOMPARE(sensor->id(), QString("orientationsensor"));

    const int INTERVAL  = 100;   // ms
    const int THRESHOLD = 100;   // mG
    const int DELAY     = 15000; // ms

    // Set and test threshold and interval
    sensor->setInterval(INTERVAL);
    QCOMPARE( sensor->interval(), INTERVAL );
    QCOMPARE( sensor->property("interval").toInt(), INTERVAL );

    sensor->setThreshold(THRESHOLD);
    QCOMPARE( sensor->threshold(), THRESHOLD );
    QCOMPARE( sensor->property("threshold").toInt(), THRESHOLD );

    connect(sensor, SIGNAL(orientationChanged(const int&)),
            &test, SLOT(handleOrientationChange(const int&)));

    sensor->start();

    qDebug() << "Waiting for" << DELAY << "mseconds";
    QTest::qWait(DELAY);

    sensor->stop();

    // Verify last state through property
    QCOMPARE(sensor->property("orientation").toInt(), (int)test.prevOrientation.orientation_);
    QCOMPARE(sensor->orientation(), (int)test.prevOrientation.orientation_);
	
    // delete sensor;
}


void SensorApiTest::testAccelerometerSensor()
{
    AccelerometerSensorChannel* sensor = dynamic_cast<AccelerometerSensorChannel*>(AccelerometerSensorChannel::factoryMethod("accelerometersensor"));
    QVERIFY ( sensor );

    TestClass test("testAccelerometerSensor");

    QCOMPARE( sensor->id(), QString("accelerometersensor") );

    const int INTERVAL    = 1000;
    const int NUM_SIGNALS = 5;

    sensor->setInterval(INTERVAL); // ms
    QCOMPARE( sensor->interval(), INTERVAL );
    QCOMPARE( sensor->property("interval").toInt(), INTERVAL );

    connect(sensor, SIGNAL(dataAvailable(const XYZ&)),
            &test, SLOT(handleAccelerationData(const XYZ&)));

    sensor->start();

    qDebug ( "Waiting %d mseconds", NUM_SIGNALS * INTERVAL );
    QTest::qWait( NUM_SIGNALS * INTERVAL + INTERVAL / 2 );

    sensor->stop();

    //TODO: This will only work after interval works with SysFs adaptors
    //QCOMPARE( test.numSamplesReceived, NUM_SIGNALS );

    //delete sensor; // does also do the release
}

void SensorApiTest::testProximitySensor()
{
    ProximitySensorChannel* sensor = dynamic_cast<ProximitySensorChannel*>(ProximitySensorChannel::factoryMethod("proximitysensor"));
    QVERIFY ( sensor );

    TestClass test("testProximitySensor");

    QCOMPARE( sensor->id(), QString("proximitysensor") );

    const int INTERVAL    = 1000;
    const int NUM_SIGNALS = 5;

    sensor->setInterval(INTERVAL); // ms
    QCOMPARE( sensor->interval(), INTERVAL );
    QCOMPARE( sensor->property("interval").toInt(), INTERVAL );

    connect(sensor, SIGNAL(dataAvailable(const int&)),
            &test, SLOT(handleProximity(const int&)));

    sensor->start();

    qDebug ( "Waiting %d mseconds", NUM_SIGNALS * INTERVAL );
    QTest::qWait( NUM_SIGNALS * INTERVAL + INTERVAL / 2 );

    sensor->stop();

    //TODO: This will only work after interval works with SysFs adaptors
    //QCOMPARE( test.numSamplesReceived, NUM_SIGNALS );

    //delete sensor; // does also do the release
}

#ifdef USE_COMPASS
void SensorApiTest::testCompassSensor()
{
    CompassSensorChannel* sensor = dynamic_cast<CompassSensorChannel*>(CompassSensorChannel::factoryMethod("compasssensor"));
    QVERIFY ( sensor );

    TestClass test("testCompassSensor");

    QCOMPARE( sensor->id(), QString("compasssensor") );

    const int INTERVAL    = 1000;
    const int NUM_SIGNALS = 5;
    const bool USE_DECLINATION = true;
    const int DECLINATION_ANGLE = 10;

    sensor->setInterval(INTERVAL); // ms
    QCOMPARE( sensor->interval(), INTERVAL );
    QCOMPARE( sensor->property("interval").toInt(), INTERVAL );

    sensor->setDeclination(USE_DECLINATION);
    QCOMPARE( sensor->declination(), USE_DECLINATION );
    QCOMPARE( sensor->property("usedeclination").toBool(), USE_DECLINATION );

    sensor->setDeclinationAngle(DECLINATION_ANGLE);
    QCOMPARE( (int)(sensor->declinationAngle()), DECLINATION_ANGLE );
    QCOMPARE( sensor->property("declinationvalue").toInt(), DECLINATION_ANGLE ); 

    connect(sensor, SIGNAL(degreesChanged(const int&)),
            &test, SLOT(handleCompassDegrees(const int&)));

    connect(sensor, SIGNAL(levelChanged(const int&)),
            &test, SLOT(handleCompassLevel(const int&)));

    sensor->start();

    qDebug ( "Waiting %d mseconds", NUM_SIGNALS * INTERVAL );
    QTest::qWait( NUM_SIGNALS * INTERVAL + INTERVAL / 2 );

    //TODO: This will only work after interval works with SysFs adaptors
    //QCOMPARE( test.numSamplesReceived, NUM_SIGNALS );

    sensor->stop();

    // Verify last state through property
    QCOMPARE(sensor->property("degrees").toInt(), test.prevCompassDegrees);
    QCOMPARE(sensor->degrees(), test.prevCompassDegrees);
    QCOMPARE(sensor->property("level").toInt(), test.prevCompassLevel);
    QCOMPARE(sensor->level(), test.prevCompassLevel);

    //delete sensor; // does also do the release
}
#endif

void SensorApiTest::testALSSensor()
{
    ALSSensorChannel* sensor = dynamic_cast<ALSSensorChannel*>(ALSSensorChannel::factoryMethod("alssensor"));
    QVERIFY ( sensor );

    TestClass test("testALSSensor");

    QCOMPARE( sensor->id(), QString("alssensor") );

    const int INTERVAL    = 5000;

    sensor->setInterval(INTERVAL); // ms
    QCOMPARE( sensor->interval(), INTERVAL );
    QCOMPARE( sensor->property("interval").toInt(), INTERVAL );

    connect(sensor, SIGNAL(ALSChanged(const int&)),
            &test, SLOT(handleALSChanged(const int&)));

    sensor->start();

    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    sensor->stop();

    // Verify last state through property
    QCOMPARE(sensor->property("lux").toInt(), test.prevALS);
    QCOMPARE(sensor->lux(), test.prevALS);

    //delete sensor; // does also do the release
}
#endif

QTEST_MAIN(SensorApiTest)
