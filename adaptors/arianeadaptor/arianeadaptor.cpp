#include <cerrno>

#include "arianeentry.h"
#include "arianeadaptor.h"
#include "timedunsigned.h"
#include "timedkey.h"

bool ArianeAdaptor::arianeLibInitialized_ = false;

void ArianeAdaptor::printSubsystemErrorStr(int rc)
{
    if ( rc < 0 )
    {
        switch ( -rc )
        {
            case ARIANE_ERROR_GENERIC :
                qDebug() << "ERROR: ARIANE_ERROR_GENERIC";
            break;
            case ARIANE_ERROR_USB :
                qDebug() << "ERROR: ARIANE_ERROR_USB";
            break;
            case ARIANE_ERROR_USB_OPEN :
                qDebug() << "ERROR: ARIANE_ERROR_USB_OPEN";
            break;
            case ARIANE_ERROR_USB_ACCESS :
                qDebug() << "ERROR: ARIANE_ERROR_USB_ACCESS";
            break;
            default :
                qDebug() << "ERROR:" << strerror( - rc );
            break;
        }
    }
}

ArianeAdaptor::ArianeAdaptor(const QString& id)
    : DeviceAdaptor(id)
    , isRunning_(false)
    , variantMode_(false)
    , dev_(NULL)
{
    if ( !arianeLibInitialized_ )
    {
        int rc = ariane_init();
        printSubsystemErrorStr(rc);
        Q_ASSERT( rc == 0 );

        arianeLibInitialized_ = true;
    }
}

ArianeAdaptor::~ArianeAdaptor()
{
    if ( dev_ )
    {
        int rc = ariane_disconnect(dev_);
        printSubsystemErrorStr(rc);
        Q_ASSERT( rc == 0 );
    }
}

void ArianeAdaptor::retrieveSensorInformation(ariane_sid_t sensorId)
{
    ariane_sensor_t* sensor = NULL;
    ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open_sensor(dev_, 0, sensorId, ARIANE_SENSOR_ACCESS_MODE_ANY, &sensor);
    if ( accessMode < 0  || accessMode == ARIANE_SENSOR_ACCESS_MODE_DENIED )
    {
        qDebug() << "ERROR: Failed to open sensor";
        Q_ASSERT( sensor == NULL );
        return;
    }

    char descriptionStr[ARIANE_MAX_SENSOR_DESCR_LEN];
    int rc = ariane_sensor_get_description(sensor, descriptionStr);
    printSubsystemErrorStr(rc);
    if ( rc != 0 )
    {
        qDebug() << "ERROR: Failed to retrieve sensor description";
        rc = ariane_sensor_close(sensor);
        printSubsystemErrorStr(rc);
        Q_ASSERT( rc == 0 );
        return;
    }
    
    QString sensorDescription(descriptionStr);
    qDebug() << "Found sensor:" << sensorDescription << "(" << sensorId << ")";

    ariane_sensor_properties_t sensorProps;
    memset(&sensorProps, 0x00, sizeof(sensorProps));
    rc = ariane_sensor_get_properties(sensor, &sensorProps);

    ArianeAdaptedSensorEntry* newHwSensor = NULL;
    if ( rc == 0 )
    {
        newHwSensor = new ArianeAdaptedSensorEntry(*this, sensorId, sensorDescription, &sensorProps);
    }
    else if ( rc == -ENOENT )
    {
        newHwSensor = new ArianeAdaptedSensorEntry(*this, sensorId, sensorDescription, NULL);
    }
    else
    {
        qDebug() << "ERROR: Failed to retrieve sensor properties";
        rc = ariane_sensor_close(sensor);
        Q_ASSERT( rc == 0 );
        return;
    }
    Q_ASSERT( newHwSensor );

    ariane_sensor_parameter_t* parameterList = NULL;
    int numParams = ariane_sensor_get_parameters(sensor, &parameterList);
    printSubsystemErrorStr(numParams);
    if ( numParams < 0 )
    {
        qDebug() << "ERROR: Failed to retrieve sensor parameters";

        if (parameterList)
        {
            free(parameterList);
        }

        rc = ariane_sensor_close(sensor);
        printSubsystemErrorStr(rc);
        Q_ASSERT( rc == 0 );
        return;
    }

    for (int i = 0; i < numParams; i++)
    {
        // retrieving the actual parameter value
        uint32_t parameterValue;
        rc = ariane_sensor_get_parameter_value(sensor, parameterList[i].paramid, &parameterValue);
        printSubsystemErrorStr(rc);
        if ( rc != 0 )
        {
            qDebug() << "ERROR: Failed to retrieve parameter value";

            if (parameterList)
            {
                free(parameterList);
            }

            rc = ariane_sensor_close(sensor);
            Q_ASSERT( rc == 0 );
            return;
        }

        newHwSensor->QObject::setProperty( parameterList[i].description, parameterValue );

        newHwSensor->paramMap_.insert( parameterList[i].description, parameterList[i] );
    }

    if (parameterList)
    {
        free(parameterList);
    }

    rc = ariane_sensor_close(sensor);
    printSubsystemErrorStr(rc);
    Q_ASSERT( rc == 0 );

    //newHwSensor->print();

    addAdaptedSensor(newHwSensor->name(), newHwSensor);
}

void ArianeAdaptor::populateRegistry()
{
    Q_ASSERT( !isRunning_ );
    Q_ASSERT( dev_ );

    ariane_sid_t sensorsIdList[ARIANE_MAX_SENSORS];
    int numSensors = ariane_sensordev_get_sensors(dev_, sensorsIdList);
    printSubsystemErrorStr(numSensors);
    Q_ASSERT( numSensors >= 0 );

    for (int i = 0; i < numSensors; i++)
    {
        retrieveSensorInformation( sensorsIdList[i] );
    }
}

bool ArianeAdaptor::startAdaptor()
{
    if ( !isRunning_ )
    {
        Q_ASSERT( dev_ == NULL );

        int rc;
        if (!btAddress_.isEmpty())
        {
            rc = ariane_bt_connect(btAddress_.toAscii().data(), &dev_);
            printSubsystemErrorStr(rc);
            if ( rc == -ENODEV )
            {
                qDebug() << "No BT device found with address" << btAddress_ << "...";
            }
        }
        else
        {
            rc = ariane_usb_connect(&dev_);
            printSubsystemErrorStr(rc);
            if ( rc == -ENODEV )
            {
                qDebug() << "No USB device found...";
            }
        }

        if (rc == 0)
        {
            populateRegistry();

            isRunning_ = true;
        }
    }

    return isRunning_;
}

void ArianeAdaptor::stopAdaptor()
{
    if ( isRunning_ && dev_ )
    {
        int numRunning = 0;
        foreach (AdaptedSensorEntry* entry, sensors_)
        {
            Q_ASSERT( entry );
            if ( entry->isRunning() )
            {
                numRunning++;
            }
        }

        if (numRunning == 0)
        {
            //qDebug() << "Stopping adaptor (no more sensors running).";

            int rc = ariane_disconnect(dev_);
            printSubsystemErrorStr(rc);
            Q_ASSERT( rc == 0 );

            dev_ = NULL;

            isRunning_ = false;
        }
        else
        {
            //qDebug() << "Other sensors still running, not stopping adaptor.";
        }
    }
    else
    {
        qWarning() << __PRETTY_FUNCTION__ << "could not stop adaptor (not connected)";
    }
}

bool ArianeAdaptor::startSensor(const QString& sensorId)
{
    if ( dev_ )
    {

        ArianeAdaptedSensorEntry* entry = dynamic_cast<ArianeAdaptedSensorEntry*>(findAdaptedSensor(sensorId));
        if ( entry == NULL )
        {
            qDebug() << __PRETTY_FUNCTION__ << "sensor" << sensorId << "not found";
            return false;
        }

        Q_ASSERT( entry->sensor_ == NULL ); // make sure it is not intitialzed before
        ariane_access_mode_t accessMode = ARIANE_SENSOR_ACCESS_MODE_CONTROL;
        int rc = ariane_sensordev_open_sensor(dev_, 0, entry->hwSensorId(), accessMode, &entry->sensor_);
        printSubsystemErrorStr(rc);
        if ( rc != 0 || accessMode != ARIANE_SENSOR_ACCESS_MODE_CONTROL )
        {
            qDebug() << __PRETTY_FUNCTION__ << "sensor" << sensorId << "can not be opened";
            return false;
        }
        entry->createRingBuffer();

        rc = ariane_sensor_start(entry->sensor_, entry->staticEventHandler, entry);
        printSubsystemErrorStr(rc);
        Q_ASSERT( rc == 0 );

        entry->setIsRunning( true );
    }
    else
    {
        qDebug() << __PRETTY_FUNCTION__ << "could not start sensor (no connection to sensor)";
        return false;
    }

    return true; // do error checking on cause
}

void ArianeAdaptor::stopSensor(const QString& sensorId)
{
    ArianeAdaptedSensorEntry* entry = dynamic_cast<ArianeAdaptedSensorEntry*>(findAdaptedSensor(sensorId));
    if ( entry  )
    {
        Q_ASSERT( entry->sensor_ != NULL ); // make sure it is intitialzed before
        int rc = ariane_sensor_stop( entry->sensor_ );
        printSubsystemErrorStr(rc);
        Q_ASSERT( rc == 0 );

        rc = ariane_sensor_close( entry->sensor_ );
        printSubsystemErrorStr(rc);
        Q_ASSERT( rc == 0 );
        entry->sensor_ = NULL;

        entry->destroyRingBuffer();
        Q_ASSERT( entry->buffer_ == NULL );

        entry->setIsRunning( false );
    }
    else
    {
        qDebug() << __PRETTY_FUNCTION__ << "sensor" << sensorId << "not found";
    }
}

void ArianeAdaptor::handleTimedXyzData(RingBufferBase* ringBuffer, uint32_t timestamp, ariane_event_xyz16_t xyz)
{
    Q_ASSERT( ringBuffer );

    DeviceAdaptorRingBuffer<TimedXyzData>* timedXyzBuffer = dynamic_cast<DeviceAdaptorRingBuffer<TimedXyzData>* >(ringBuffer);
    Q_ASSERT( timedXyzBuffer );

    TimedXyzData* timedXyzData = timedXyzBuffer->nextSlot();
    Q_ASSERT( timedXyzData );

    timedXyzData->timestamp_ = timestamp;
    timedXyzData->x_         = xyz.x;
    timedXyzData->y_         = xyz.y;
    timedXyzData->z_         = xyz.z;

    /*
    qDebug() << __PRETTY_FUNCTION__
             << timedXyzData->timestamp_
             << timedXyzData->x_
             << timedXyzData->y_
             << timedXyzData->z_;
    */

    timedXyzBuffer->commit();
    timedXyzBuffer->wakeUpReaders();
}

void ArianeAdaptor::handleTimedUnsigned(RingBufferBase* ringBuffer, uint32_t timestamp, unsigned data)
{
    Q_ASSERT( ringBuffer );

    DeviceAdaptorRingBuffer<TimedUnsigned>* timedUnsignedBuffer = dynamic_cast<DeviceAdaptorRingBuffer<TimedUnsigned>* >(ringBuffer);
    Q_ASSERT( timedUnsignedBuffer );

    TimedUnsigned* timedUnsigned = timedUnsignedBuffer->nextSlot();

    timedUnsigned->timestamp_ = timestamp;
    timedUnsigned->value_     = data;

    /*
    qDebug() << __PRETTY_FUNCTION__
             << timedUnsigned->timestamp_
             << timedUnsigned->value_;
    */

    timedUnsignedBuffer->commit();
    timedUnsignedBuffer->wakeUpReaders();
}

void ArianeAdaptor::handleTimedKey(RingBufferBase* ringBuffer, uint32_t timestamp, ariane_event_key_t data)
{
    Q_ASSERT( ringBuffer );

    DeviceAdaptorRingBuffer<TimedKey>* timedKeyBuffer = dynamic_cast<DeviceAdaptorRingBuffer<TimedKey>* >(ringBuffer);
    Q_ASSERT( timedKeyBuffer );

    TimedKey* timedKey = timedKeyBuffer->nextSlot();

    timedKey->timestamp_ = timestamp;
    timedKey->value_     = data;

    /*
    qDebug() << __PRETTY_FUNCTION__
             << timedKey->timestamp_
             << timedKey->value_.key
             << timedKey->value_.pressed
             << timedKey->value_.released;
    */

    timedKeyBuffer->commit();
    timedKeyBuffer->wakeUpReaders();
}

void ArianeAdaptor::handleTimedXyzDataVariant(RingBufferBase* ringBuffer, uint32_t timestamp, ariane_event_xyz16_t xyz)
{
    Q_ASSERT( ringBuffer );

    DeviceAdaptorRingBuffer<QVariant>* variantBuffer = dynamic_cast<DeviceAdaptorRingBuffer<QVariant>* >(ringBuffer);
    Q_ASSERT( variantBuffer );

    QVariant* variant = variantBuffer->nextSlot();

    TimedXyzData timedXyzData;
    timedXyzData.timestamp_ = timestamp;
    timedXyzData.x_         = xyz.x;
    timedXyzData.y_         = xyz.y;
    timedXyzData.z_         = xyz.z;

    /*
    qDebug() << __PRETTY_FUNCTION__
             << timedXyzData.timestamp_
             << timedXyzData.x_
             << timedXyzData.y_
             << timedXyzData.z_;
    */

    variant->setValue(timedXyzData);

    variantBuffer->commit();
    variantBuffer->wakeUpReaders();
}

void ArianeAdaptor::handleTimedUnsignedVariant(RingBufferBase* ringBuffer, uint32_t timestamp, unsigned data)
{
    Q_ASSERT( ringBuffer );

    DeviceAdaptorRingBuffer<QVariant>* variantBuffer = dynamic_cast<DeviceAdaptorRingBuffer<QVariant>* >(ringBuffer);
    Q_ASSERT( variantBuffer );

    QVariant* variant = variantBuffer->nextSlot();

    TimedUnsigned timedUnsigned;
    timedUnsigned.timestamp_ = timestamp;
    timedUnsigned.value_     = data;

    /*
    qDebug() << __PRETTY_FUNCTION__
             << timedUnsigned.timestamp_
             << timedUnsigned.value_;
    */

    variant->setValue(timedUnsigned);

    variantBuffer->commit();
    variantBuffer->wakeUpReaders();
}

void ArianeAdaptor::handleTimedKeyVariant(RingBufferBase* ringBuffer, uint32_t timestamp, ariane_event_key_t data)
{
    Q_ASSERT( ringBuffer );

    DeviceAdaptorRingBuffer<QVariant>* variantBuffer = dynamic_cast<DeviceAdaptorRingBuffer<QVariant>* >(ringBuffer);
    Q_ASSERT( variantBuffer );

    QVariant* variant = variantBuffer->nextSlot();

    TimedKey timedKey;
    timedKey.timestamp_ = timestamp;
    timedKey.value_     = data;

    /*
    qDebug() << __PRETTY_FUNCTION__
             << timedKey.timestamp_
             << timedKey.value_.key
             << timedKey.value_.pressed
             << timedKey.value_.released;
    */

    variant->setValue(timedKey);

    variantBuffer->commit();
    variantBuffer->wakeUpReaders();
}
