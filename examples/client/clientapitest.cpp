#include "qt-api/sensormanagerinterface.h"

#ifdef USE_INTERNAL
#ifdef USE_COMPASS
#include "qt-api/compasssensor_i.h"
#endif
#include "qt-api/orientationsensor_i.h"
#include "qt-api/accelerometersensor_i.h"
#include "qt-api/alssensor_i.h"
#include "qt-api/tapsensor_i.h"
#endif

#include "testclass.h"
#ifdef USE_ARIANE
#include "qt-api/filteredsensor_i.h"
#include "qt-api/arianesensor_i.h"
#include "gesturehandler.h"
#endif

#include "clientapitest.h"

//#define USE_STATE_SIGNALS

void ClientApiTest::initTestCase()
{
    // called before everything else (in this case to register types)

    qDBusRegisterMetaType<Orientation>();
    qDBusRegisterMetaType<XYZ>();
    qDBusRegisterMetaType<Unsigned>();
#ifdef USE_ARIANE
    qDBusRegisterMetaType<Key>();
#endif
#ifdef USE_INTERNAL
    qDBusRegisterMetaType<Tap>();
#endif

    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
    QVERIFY( remoteSensorManager.isValid() );

#ifdef USE_ARIANE
    remoteSensorManager.registerSensorInterface<FilteredSensorChannelInterface>("filteredsensor");

    remoteSensorManager.registerSensorInterface<ArianeSensorChannelInterface>("arianesensor");
    //remoteSensorManager.registerSensorInterface<ArianeSensorChannelInterface>("arianesensor2");
    //remoteSensorManager.registerSensorInterface<ArianeSensorChannelInterface>("arianesensor3");
#endif

    // TODO: Unload plugins after running the tests 
    // Sensors linked statically until plugin structure has been finished
#ifdef USE_INTERNAL
#ifdef USE_COMPASS
    remoteSensorManager.registerSensorInterface<CompassSensorChannelInterface>("compasssensor");
#endif
    remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");
    remoteSensorManager.registerSensorInterface<OrientationSensorChannelInterface>("orientationsensor");
    remoteSensorManager.registerSensorInterface<ALSSensorChannelInterface>("alssensor");
    remoteSensorManager.registerSensorInterface<TapSensorChannelInterface>("tapsensor");
#endif
}

void ClientApiTest::testSensorManagement()
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );


    TestErrorSignalReceiver errorReceiver;
    //bool ok = connect(&sm, SIGNAL(error(SensorManagerError)), &errorReceiver, SLOT(errorSlot(SensorManagerError)));
    bool ok = connect(&sm, SIGNAL(errorSignal(int)), &errorReceiver, SLOT(errorSlot(int)));
    QVERIFY ( ok );

#ifdef CHECK_ERROR_SIGNALS
    int numErrorSignals = 0;
#endif

    // case: request an unknown control sensor (fails)
    int controlSessionId1 = sm.requestControlSensor("unknownsensor;a=1,b=2,c=3");
    QCOMPARE( controlSessionId1, INVALID_SESSION);
    QCOMPARE( sm.errorCode(), SmIdNotRegistered );
    QVERIFY(  !sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );
#endif

    // case: request an existing listen sensor that is not controlled (fails)
    int listenSessionId1 = sm.requestListenSensor("arianesensor;a=1,b=2,c=3");
    QCOMPARE( listenSessionId1, INVALID_SESSION );
    QCOMPARE( sm.errorCode(), SmNotInstantiated );
    QVERIFY(  !sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.errorCode(), SmNotInstantiated );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );
#endif

    // case: request an existing control sensor (succeeds)
    controlSessionId1 = sm.requestControlSensor("arianesensor;a=1,b=2,c=3");
    QVERIFY( controlSessionId1 >= 0);
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );
#endif

    // case: request a second existing control sensor (fails)
    int controlSessionId2 = sm.requestControlSensor("arianesensor;a=1,b=2,c=3");
    QCOMPARE( controlSessionId2, INVALID_SESSION);
    QCOMPARE( sm.errorCode(), SmAlreadyUnderControl );
    QVERIFY(  !sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.errorCode(), SmAlreadyUnderControl );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );
#endif

    // case: request a second existing listen sensor that is controlled (succeeds)
    listenSessionId1 = sm.requestListenSensor("arianesensor;a=1,b=2,c=3");
    QVERIFY( listenSessionId1 >= 0);
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );
#endif

    // case: release existing control sensor, but wrong session ID (fails)
    ok = sm.releaseSensor("unknownsensor", controlSessionId1);
    QVERIFY( !ok );
    QCOMPARE( sm.errorCode(), SmIdNotRegistered );
    QVERIFY(  !sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );
#endif

    // case: release existing control sensor, with correct session ID (succeeds)
    ok = sm.releaseSensor("arianesensor", controlSessionId1);
    QVERIFY( ok );
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );
#endif

    // case: release second non-existing control sensor (fails)
    ok = sm.releaseSensor("arianesensor", controlSessionId2);
    QVERIFY( !ok );
    QCOMPARE( sm.errorCode(), SmNotInstantiated );
    QVERIFY(  !sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.errorCode(), SmNotInstantiated );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );
#endif

    // case: request a second existing listen sensor (succeeds)
    int listenSessionId2 = sm.requestListenSensor("arianesensor;a=1,b=2,c=3");
    QVERIFY(listenSessionId2 >= 0);
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );
#endif

    // case: request a non-existing listen sensor (fails)
    int listenSessionId3 = sm.requestListenSensor("unknownsensor;a=1,b=2,c=3");
    QCOMPARE( listenSessionId3, INVALID_SESSION);
    QCOMPARE( sm.errorCode(), SmIdNotRegistered );
    QVERIFY(  !sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );
#endif

    // case: release first existing listen sensor (succeeds)
    ok = sm.releaseSensor("arianesensor", listenSessionId1);
    QVERIFY( ok );
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );
#endif

    // case: request a (new) first existing control sensor (succeeds)
    int controlSessionId3 = sm.requestControlSensor("arianesensor;a=4,b=5,c=6");
    QVERIFY(controlSessionId3 >= 0);
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );
#endif

    // case: release second existing listen sensor (succeeds)
    ok = sm.releaseSensor("arianesensor", listenSessionId2);
    QVERIFY( ok );
    QCOMPARE( sm.errorCode(), SmNoError );
    QVERIFY(  sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );
#endif

    // case: release third already released listen sensor (fails)
    ok = sm.releaseSensor("arianesensor", listenSessionId2);
    QVERIFY( !ok );
    QCOMPARE( sm.errorCode(), SmNotInstantiated );
    QVERIFY(  !sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.errorCode(), SmNotInstantiated );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );
#endif

    // case: release third non-existing listen sensor (fails)
    ok = sm.releaseSensor("unknownsensor", listenSessionId3);
    QVERIFY( !ok );
    QCOMPARE( sm.errorCode(), SmIdNotRegistered );
    QVERIFY(  !sm.errorString().isEmpty() );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.errorCode(), SmIdNotRegistered );
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, ++numErrorSignals );
#endif

    // case: release third non-existing listen sensor (fails)
    ok = sm.releaseSensor("arianesensor", controlSessionId3);
    QVERIFY( ok );
    QVERIFY(  sm.errorString().isEmpty() );
    QCOMPARE( sm.errorCode(), SmNoError );
#ifdef CHECK_ERROR_SIGNALS
    QCOMPARE( errorReceiver.numErrorSignalsReceived_, numErrorSignals );
#endif
}

#ifdef USE_ARIANE
void ClientApiTest::testFilteredSensorControl()
{
    FilteredSensorChannelInterface* sensorIfc = FilteredSensorChannelInterface::controlInterface("filteredsensor");
    QVERIFY (sensorIfc && sensorIfc->isValid() );

    TestClass test("test control");

    QCOMPARE( sensorIfc->id(), QString("filteredsensor") );

#ifdef USE_STATE_SIGNALS
    QVERIFY( sensorIfc->state() == STOPPED );
    QCOMPARE( sensorIfc->property("state"), "STOPPED" );

    QCOMPARE( test.currentState, STOPPED );
#endif


    connect(sensorIfc, SIGNAL(dataAvailable(const Orientation&)),
            &test, SLOT(handleData(const Orientation&)));

#ifdef USE_STATE_SIGNALS
    connect(sensorIfc, SIGNAL(stateChanged(SensorState)),
            &test, SLOT(handleStateChanged(SensorState)));

    QCOMPARE( test.currentState, STOPPED );
#endif

    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc->errorCode(), SNoError );

#ifdef USE_STATE_SIGNALS
    QCOMPARE( test.currentState, STARTED );
#endif

    const int DURATION = 1000;
    qDebug ( "Waiting %d mseconds", DURATION );
    QTest::qWait( DURATION );

    QVERIFY( test.numSamplesReceived > 0 );

    reply = sensorIfc->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc->errorCode(), SNoError );

    bool result = sensorIfc->release();
    QVERIFY ( result );

    delete sensorIfc; // does also do the release
}

void ClientApiTest::testArianeTapSensor()
{
    const QString TAP_VARIANT_SENSOR_ID = "arianesensor;hardwareSensorId=1809";
    ArianeSensorChannelInterface* sensorIfc = ArianeSensorChannelInterface::controlInterface(TAP_VARIANT_SENSOR_ID);
    QVERIFY (sensorIfc && sensorIfc->isValid() );

    QCOMPARE( sensorIfc->id(), QString("arianesensor") );

    /*
    param id	 = 2
    description	 = ' Bit flag options '
    min value	 = 0
    max value	 = 255
    flags	 = 2 ( PARAM_FLAG_READONLY = false , PARAM_FLAG_SIGNED = false  )
    value (cached)   = 3
    */
    const QString  PROP_NAME_BIT_FLAG_OPTIONS  = "Bit flag options";
    const unsigned PROP_VALUE_BIT_FLAG_OPTIONS = 3;
    QDBusReply<bool> boolReply = sensorIfc->setAdaptorProperty( PROP_NAME_BIT_FLAG_OPTIONS, PROP_VALUE_BIT_FLAG_OPTIONS );
    QVERIFY( boolReply.isValid() );

    QDBusReply<unsigned> unsignedReply = sensorIfc->adaptorProperty( PROP_NAME_BIT_FLAG_OPTIONS);
    QVERIFY( unsignedReply.isValid() );
    QCOMPARE( unsignedReply.value(), PROP_VALUE_BIT_FLAG_OPTIONS );

    /*
    param id	 = 3
    description	 = ' Deactivation timeout (ms) '
    min value	 = 500
    max value	 = 4294967295
    flags	 = 2 ( PARAM_FLAG_READONLY = false , PARAM_FLAG_SIGNED = false  )
    value (cached)   = 2000
    */
    const QString  PROP_NAME_DEACT_TIMEOUT  = "Deactivation timeout (ms)";
    const unsigned PROP_VALUE_DEACT_TIMEOUT = 3000;
    boolReply = sensorIfc->setAdaptorProperty( PROP_NAME_DEACT_TIMEOUT, PROP_VALUE_DEACT_TIMEOUT );
    QVERIFY( boolReply.isValid() );

    unsignedReply = sensorIfc->adaptorProperty( PROP_NAME_DEACT_TIMEOUT );
    QVERIFY( unsignedReply.isValid() );
    QCOMPARE( unsignedReply.value(), PROP_VALUE_DEACT_TIMEOUT );

    /*
    param id	 = 1
    description	 = ' Filter bits '
    min value	 = 1
    max value	 = 10
    flags	 = 2 ( PARAM_FLAG_READONLY = false , PARAM_FLAG_SIGNED = false  )
    value (cached)   = 6
    */
    const QString  PROP_NAME_FILTER_BITS  = "Filter bits";
    const unsigned PROP_VALUE_FILTER_BITS = 6;
    boolReply = sensorIfc->setAdaptorProperty( PROP_NAME_FILTER_BITS, PROP_VALUE_FILTER_BITS );
    QVERIFY( boolReply.isValid() );

    unsignedReply = sensorIfc->adaptorProperty( PROP_NAME_FILTER_BITS );
    QVERIFY( unsignedReply.isValid() );
    QCOMPARE( unsignedReply.value(), PROP_VALUE_FILTER_BITS );

    /*
    param id	 = 0
    description	 = ' Tap threshold '
    min value	 = 0
    max value	 = 32767
    flags	 = 2 ( PARAM_FLAG_READONLY = false , PARAM_FLAG_SIGNED = false  )
    value (cached)   = 20000
    */
    const QString  PROP_NAME_TAP_THRESHOLD  = "Tap threshold";
    const unsigned PROP_VALUE_TAP_THRESHOLD = 25000;
    boolReply = sensorIfc->setAdaptorProperty( PROP_NAME_TAP_THRESHOLD, PROP_VALUE_TAP_THRESHOLD );
    QVERIFY( boolReply.isValid() );

    unsignedReply = sensorIfc->adaptorProperty( PROP_NAME_TAP_THRESHOLD );
    QVERIFY( unsignedReply.isValid() );
    QCOMPARE( unsignedReply.value(), PROP_VALUE_TAP_THRESHOLD );


    TestClass test("testArianeTapSensor");

    bool ok = connect(sensorIfc, SIGNAL(dataAvailable(const Key&)),
                      &test, SLOT(handleData(const Key&)));
    QVERIFY( ok );


    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc->errorCode(), SNoError );

    const int WAITING_DELAY = 5000;
    qDebug ( "Waiting %d mseconds", WAITING_DELAY );
    QTest::qWait( WAITING_DELAY );

    qDebug() << "Number of received samples:" << test.numSamplesReceived;
    //QVERIFY( test.numSamplesReceived > 0 );


    reply = sensorIfc->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc->errorCode(), SNoError );

    delete sensorIfc;
}

void ClientApiTest::testArianeSensorControl()
{
    ArianeSensorChannelInterface* sensorIfc = ArianeSensorChannelInterface::controlInterface("arianesensor");
    QVERIFY (sensorIfc && sensorIfc->isValid() );

    QCOMPARE( sensorIfc->id(), QString("arianesensor") );

    const int SENSOR_ID_1 = 1040;

    sensorIfc->setHardwareSensorId( SENSOR_ID_1 );
    QCOMPARE( sensorIfc->hardwareSensorId(), SENSOR_ID_1 );
    QCOMPARE( sensorIfc->property("hardwareSensorId").toInt(), SENSOR_ID_1 );

    const int INTERVAL      = 50;
    const int NUM_SIGNALS_1 = 10;

    sensorIfc->setInterval(INTERVAL); // ms
    QCOMPARE( sensorIfc->interval(), INTERVAL );
    QCOMPARE( sensorIfc->property("interval").toInt(), INTERVAL );

    TestClass test1("FirstTest");

    bool ok = connect(sensorIfc, SIGNAL(dataAvailable(const XYZ&)),
                      &test1, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );

    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc->errorCode(), SNoError );


    qDebug ( "Waiting %d mseconds", NUM_SIGNALS_1 * INTERVAL );
    QTest::qWait( NUM_SIGNALS_1 * INTERVAL + INTERVAL / 2 );

    QVERIFY( test1.numSamplesReceived > 0 );

    qDebug() << "Number of received samples:" << test1.numSamplesReceived;


    reply = sensorIfc->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc->errorCode(), SNoError );

    const int SENSOR_ID_2 = 784;

    sensorIfc->setHardwareSensorId( SENSOR_ID_2 );
    QCOMPARE( sensorIfc->hardwareSensorId(), SENSOR_ID_2 );
    QCOMPARE( sensorIfc->property("hardwareSensorId").toInt(), SENSOR_ID_2 );

    TestClass test2("SecondTest");

    ok = connect(sensorIfc, SIGNAL(dataAvailable(const Unsigned&)),
                 &test2, SLOT(handleData(const Unsigned&)));
    QVERIFY( ok );

    reply = sensorIfc->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc->errorCode(), SNoError );

    const int NUM_SIGNALS_2 = 100;
    qDebug ( "Waiting %d mseconds", NUM_SIGNALS_2 * INTERVAL );
    QTest::qWait( NUM_SIGNALS_2 * INTERVAL + INTERVAL / 2 );

    QVERIFY( test2.numSamplesReceived > 0 );

    reply = sensorIfc->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc->errorCode(), SNoError );

    delete sensorIfc; // does also do the release
}

void ClientApiTest::testArianeMultipleSensorControl()
{
    const int MAG_SENSOR_ID = 1040;
    const int MAG_INTERVAL = 1000/10;
    TestClass magTest("MagneticField");

    ArianeSensorChannelInterface* sensorIfc_mag = ArianeSensorChannelInterface::controlInterface("arianesensor");
    QVERIFY ( sensorIfc_mag && sensorIfc_mag->isValid() );
    sensorIfc_mag->setHardwareSensorId(MAG_SENSOR_ID);
    sensorIfc_mag->setInterval(MAG_INTERVAL); // ms
    bool ok = connect(sensorIfc_mag, SIGNAL(dataAvailable(const XYZ&)),
                      &magTest, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );
    QDBusReply<void> reply = sensorIfc_mag->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc_mag->errorCode(), SNoError );


    const int ACC_SENSOR_ID = 16;
    const int ACC_INTERVAL = 1000/80;
    TestClass accTest("Accelerometer");

    ArianeSensorChannelInterface* sensorIfc_acc = ArianeSensorChannelInterface::controlInterface("arianesensor2");
    QVERIFY ( sensorIfc_acc && sensorIfc_acc->isValid() );
    sensorIfc_acc->setHardwareSensorId(ACC_SENSOR_ID);
    sensorIfc_acc->setInterval(ACC_INTERVAL); // ms
    ok = connect(sensorIfc_acc, SIGNAL(dataAvailable(const XYZ&)),
                 &accTest, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );
    reply = sensorIfc_acc->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc_acc->errorCode(), SNoError );


    const int GYRO_SENSOR_ID = 272;
    const int GYR_INTERVAL = 1000/40;
    TestClass gyrTest("AngularRate");

    ArianeSensorChannelInterface* sensorIfc_gyr = ArianeSensorChannelInterface::controlInterface("arianesensor3");
    QVERIFY ( sensorIfc_gyr && sensorIfc_gyr->isValid() );
    sensorIfc_gyr->setHardwareSensorId(GYRO_SENSOR_ID);
    sensorIfc_gyr->setInterval(GYR_INTERVAL); // ms
    ok = connect(sensorIfc_gyr, SIGNAL(dataAvailable(const XYZ&)),
                 &gyrTest, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );
    reply = sensorIfc_gyr->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc_gyr->errorCode(), SNoError );


    const int WAIT_DELAY = 500;
    qDebug ( "Waiting %d mseconds", WAIT_DELAY );
    QTest::qWait( WAIT_DELAY );


    qDebug() << "Samples received by magTest" << magTest.numSamplesReceived;
    qDebug() << "Samples received by accTest" << accTest.numSamplesReceived;
    qDebug() << "Samples received by gyrTest" << gyrTest.numSamplesReceived;

    QVERIFY( magTest.numSamplesReceived > 0 );
    QVERIFY( accTest.numSamplesReceived > 0 );
    QVERIFY( gyrTest.numSamplesReceived > 0 );

    reply = sensorIfc_mag->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc_mag->errorCode(), SNoError );

    ok = sensorIfc_mag->release();
    QVERIFY( ok );

    reply = sensorIfc_acc->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc_acc->errorCode(), SNoError );

    ok = sensorIfc_acc->release();
    QVERIFY( ok );

    reply = sensorIfc_gyr->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc_gyr->errorCode(), SNoError );

    ok = sensorIfc_gyr->release();
    QVERIFY( ok );
}

void ClientApiTest::testArianeTwoSensorBoxes()
{
    const QString TAP_VARIANT_SENSOR_ID_1 = "arianesensor;hardwareSensorId=2064,adaptor=arianeadaptor";
    ArianeSensorChannelInterface* sensorIfc1 = ArianeSensorChannelInterface::controlInterface(TAP_VARIANT_SENSOR_ID_1);
    QVERIFY (sensorIfc1 && sensorIfc1->isValid() );

    QCOMPARE( sensorIfc1->id(), QString("arianesensor") );

    const QString TAP_VARIANT_SENSOR_ID_2 = "arianesensor2;hardwareSensorId=2064,bluetoothAddress=00:02:5B:00:A7:00,adaptor=arianeadaptor2";
    ArianeSensorChannelInterface* sensorIfc2 = ArianeSensorChannelInterface::controlInterface(TAP_VARIANT_SENSOR_ID_2);
    QVERIFY (sensorIfc2 && sensorIfc2->isValid() );

    QCOMPARE( sensorIfc2->id(), QString("arianesensor2") );

    TestClass test1("Sensor1");
    bool ok = connect(sensorIfc1, SIGNAL(dataAvailable(const XYZ&)),
                      &test1, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );
    QDBusReply<void> reply = sensorIfc1->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc1->errorCode(), SNoError );

    TestClass test2("Sensor2");
    ok = connect(sensorIfc2, SIGNAL(dataAvailable(const XYZ&)),
                 &test2, SLOT(handleData(const XYZ&)));
    QVERIFY( ok );
    reply = sensorIfc2->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc2->errorCode(), SNoError );


    const int WAIT_DELAY = 5000;
    qDebug ( "Waiting %d mseconds", WAIT_DELAY );
    QTest::qWait( WAIT_DELAY );

    qDebug() << "Samples received by sensor 1" << test1.numSamplesReceived;
    qDebug() << "Samples received by sensor 2" << test2.numSamplesReceived;

    QVERIFY( test1.numSamplesReceived > 0 );
    QVERIFY( test2.numSamplesReceived > 0 );


    reply = sensorIfc1->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc1->errorCode(), SNoError );

    ok = sensorIfc1->release();
    QVERIFY( ok );

    reply = sensorIfc2->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( sensorIfc2->errorCode(), SNoError );

    ok = sensorIfc2->release();
    QVERIFY( ok );
}

void ClientApiTest::testArianeVibra()
{
    ArianeSensorChannelInterface* sensorIfc = ArianeSensorChannelInterface::controlInterface("arianesensor");
    QVERIFY (sensorIfc && sensorIfc->isValid() );

    QCOMPARE( sensorIfc->id(), QString("arianesensor") );

    const int VIBRA_SENSOR_ID = ARIANE_SID(0, ARIANE_DEVICE_LOCATION_WRIST, ARIANE_MTYPE_HAPTIC);

    sensorIfc->setHardwareSensorId( VIBRA_SENSOR_ID );
    QCOMPARE( sensorIfc->hardwareSensorId(), VIBRA_SENSOR_ID );

    const int VIBRA_DURATION = 500;
    const int NUM_SIGNALS    = 3;

    for (int i = 0; i < NUM_SIGNALS; i++)
    {
        QDBusReply<void> reply = sensorIfc->start();
        QVERIFY( reply.isValid() );
        QCOMPARE ( sensorIfc->errorCode(), SNoError );

        qDebug ( "Vibrating %d mseconds", VIBRA_DURATION );
        QTest::qWait( VIBRA_DURATION );

        reply = sensorIfc->stop();
        QVERIFY( reply.isValid() );
        QCOMPARE ( sensorIfc->errorCode(), SNoError );

        qDebug ( "Waiting %d mseconds", VIBRA_DURATION );
        QTest::qWait( VIBRA_DURATION );
    }

    delete sensorIfc; // does also do the release
}

void ClientApiTest::testArianeGestures()
{
    const QString TAP_SENSOR = "arianesensor;hardwareSensorId=" + QString::number(0x711);
    ArianeSensorChannelInterface* tapSensorIfc = ArianeSensorChannelInterface::controlInterface(TAP_SENSOR);
    QVERIFY (tapSensorIfc && tapSensorIfc->isValid() );

    const QString FAKE_ORIENTATION_SENSOR = "arianesensor2;hardwareSensorId=" + QString::number(0x811);
    ArianeSensorChannelInterface* fakeOrientationIfc = ArianeSensorChannelInterface::controlInterface(FAKE_ORIENTATION_SENSOR);
    QVERIFY (fakeOrientationIfc && fakeOrientationIfc->isValid() );

    GestureHandler test;
    bool ok = connect(tapSensorIfc, SIGNAL(dataAvailable(const Key&)),
                      &test, SLOT(handleData(const Key&)));
    QVERIFY( ok );

    ok = connect(fakeOrientationIfc, SIGNAL(dataAvailable(const XYZ&)),
                 &test, SLOT(handleData(const XYZ&)));

    QDBusReply<void> reply = tapSensorIfc->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( tapSensorIfc->errorCode(), SNoError );

    reply = fakeOrientationIfc->start();
    QVERIFY( reply.isValid() );
    QCOMPARE ( fakeOrientationIfc->errorCode(), SNoError );

    const int SENSING_DURATION = 10000;
    qDebug ( "Detecting %d mseconds", SENSING_DURATION );
    QTest::qWait( SENSING_DURATION );

    reply = fakeOrientationIfc->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( fakeOrientationIfc->errorCode(), SNoError );

    reply = tapSensorIfc->stop();
    QVERIFY( reply.isValid() );
    QCOMPARE ( tapSensorIfc->errorCode(), SNoError );


    delete tapSensorIfc;
    delete fakeOrientationIfc;
}
#endif

#ifdef USE_INTERNAL
#ifdef USE_COMPASS
void ClientApiTest::testCompassSensorControl() {

    CompassSensorChannelInterface* sensorIfc = CompassSensorChannelInterface::controlInterface("compasssensor");
    QVERIFY (sensorIfc && sensorIfc->isValid());

    QVERIFY( sensorIfc->id() == "compasssensor" );

    const int INTERVAL    = 1000;
    const int DECLINATION = 32;
    const int DELAY       = 5000;

    // Property setting
    sensorIfc->setInterval(INTERVAL);
    QCOMPARE( sensorIfc->interval(), INTERVAL );
    QCOMPARE( sensorIfc->property("interval").toInt(), INTERVAL );

    //sensorIfc->setDeclination(DECLINATION);
    //QCOMPARE( sensorIfc->declination(), DECLINATION );
    //QCOMPARE( sensorIfc->property("declination").toInt(), DECLINATION );

    TestClass test("test compass control");

    connect(sensorIfc, SIGNAL(degreesChanged(const int&)),
            &test, SLOT(handleCompassDegrees(const int&)));

    connect(sensorIfc, SIGNAL(levelChanged(const int&)),
            &test, SLOT(handleCompassLevel(const int&)));

    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY (reply.isValid());

    qDebug() << "Waiting for" << DELAY << "msec.";
    QTest::qWait( DELAY );

    reply = sensorIfc->stop();
    QVERIFY (reply.isValid());

    qDebug() << "Waiting for 1 sec. to allow sync";
    QTest::qWait( 1000 );

    // Query the current measurement through properties
    QCOMPARE(sensorIfc->property("degrees").toInt(), test.prevCompassDegrees);
    QCOMPARE(sensorIfc->degrees(), test.prevCompassDegrees);
    QCOMPARE(sensorIfc->property("level").toInt(), test.prevCompassLevel);
    QCOMPARE(sensorIfc->level(), test.prevCompassLevel);

    delete sensorIfc;
}
#endif

void ClientApiTest::testAccelerometerSensorControl()
{
	AccelerometerSensorChannelInterface* sensorIfc = AccelerometerSensorChannelInterface::controlInterface("accelerometersensor");
    QVERIFY (sensorIfc && sensorIfc->isValid());

    QVERIFY( sensorIfc->id() == "accelerometersensor" );

    const int INTERVAL    = 1000;
	const int NUM_SIGNALS = 5;

    // Property setting
    sensorIfc->setInterval(INTERVAL);
    QCOMPARE( sensorIfc->interval(), INTERVAL );
    QCOMPARE( sensorIfc->property("interval").toInt(), INTERVAL );

    TestClass test("test accelerometer control");

    connect(sensorIfc, SIGNAL(dataAvailable(const XYZ&)),
            &test, SLOT(handleAccelerationData(const XYZ&)));

    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY (reply.isValid());

    qDebug() << "Waiting for" << NUM_SIGNALS * INTERVAL << "msec.";
    QTest::qWait( NUM_SIGNALS * INTERVAL );

    reply = sensorIfc->stop();
    QVERIFY (reply.isValid());

	// TODO: Verify sample count (only after interval has an effect)
    //QCOMPARE( test.numSamplesReceived, NUM_SIGNALS );

    delete sensorIfc;
}

void ClientApiTest::testOrientationSensorControl()
{
	OrientationSensorChannelInterface* sensorIfc = OrientationSensorChannelInterface::controlInterface("orientationsensor");
    QVERIFY (sensorIfc && sensorIfc->isValid());

    QVERIFY( sensorIfc->id() == "orientationsensor" );

	const int INTERVAL    = 1000;
	const int DELAY       = 50000;
	const int THRESHOLD   = 25;
	
	// Property setting
    sensorIfc->setInterval(INTERVAL);
    QCOMPARE( sensorIfc->interval(), INTERVAL );
    QCOMPARE( sensorIfc->property("interval").toInt(), INTERVAL );

	sensorIfc->setThreshold(THRESHOLD);
    QCOMPARE( sensorIfc->threshold(), THRESHOLD );
    QCOMPARE( sensorIfc->property("threshold").toInt(), THRESHOLD );

    TestClass test("test accelerometer control");

	connect(sensorIfc, SIGNAL(orientationChanged(const int&)), 
	        &test, SLOT(handleOrientationChange(const int&)));

    QDBusReply<void> reply = sensorIfc->start();
    QVERIFY (reply.isValid());

	// Can't expect any frequency - needs device movement to signal.
    qDebug() << "Waiting for" << DELAY << "msec.";
    QTest::qWait( DELAY );

    reply = sensorIfc->stop();
    QVERIFY (reply.isValid());

	// Test last state through properties
    QCOMPARE(sensorIfc->property("orientation").toInt(), (int)test.prevOrientation.orientation_);
    QCOMPARE(sensorIfc->orientation(), (int)test.prevOrientation.orientation_);
	
	delete sensorIfc;
}

void ClientApiTest::testALSSensorControl()
{
    ALSSensorChannelInterface* sensorIfc = ALSSensorChannelInterface::controlInterface("alssensor");
    QVERIFY (sensorIfc && sensorIfc->isValid());

    QVERIFY( sensorIfc->id() == "alssensor" );

    TestClass test("test ALS control");

    const int INTERVAL    = 5000;

    sensorIfc->setInterval(INTERVAL); // ms
    QCOMPARE( sensorIfc->interval(), INTERVAL );
    QCOMPARE( sensorIfc->property("interval").toInt(), INTERVAL );

    connect(sensorIfc, SIGNAL(ALSChanged(const int&)),
            &test, SLOT(handleALSChanged(const int&)));

    sensorIfc->start();

    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    sensorIfc->stop();

	// Verify last state through property
    QCOMPARE(sensorIfc->property("lux").toInt(), test.prevALS);
    QCOMPARE(sensorIfc->lux(), test.prevALS);

	delete sensorIfc;
}

void ClientApiTest::testTapSensorControl()
{
    TapSensorChannelInterface* sensorIfc = TapSensorChannelInterface::controlInterface("tapsensor");
    QVERIFY (sensorIfc && sensorIfc->isValid());

    QVERIFY( sensorIfc->id() == "tapsensor" );

    TestClass test("test TAP control");

    const int INTERVAL    = 5000;

    sensorIfc->setInterval(INTERVAL); // ms
    QCOMPARE( sensorIfc->interval(), INTERVAL );
    QCOMPARE( sensorIfc->property("interval").toInt(), INTERVAL );

    connect(sensorIfc, SIGNAL(dataAvailable(const Tap&)),
            &test, SLOT(handleTapEvent(const Tap&)));

    sensorIfc->start();

    qDebug ( "Waiting %d mseconds", INTERVAL );
    QTest::qWait( INTERVAL );

    sensorIfc->stop();

	// Verify last state through property
    //QCOMPARE(sensorIfc->property("lux").toInt(), test.prevALS);
    //QCOMPARE(sensorIfc->lux(), test.prevALS);

	delete sensorIfc;
}
#endif

QTEST_MAIN(ClientApiTest)
