#include <QtDebug>
#include <QVariant>

#include "arianeadaptor.h"
#include "arianeentry.h"
#include "timedunsigned.h"
#include "timedkey.h"

const char* ArianeAdaptedSensorEntry::ARIANE_PARAM_NAME_SAMPLING_RATE = "Sampling rate";

ArianeAdaptedSensorEntry::ArianeAdaptedSensorEntry(ArianeAdaptor& adaptor,
                                                   ariane_session_id_t hwSensorId,
                                                   const QString& description,
                                                   const ariane_sensor_properties_t* sensorProps)
    : AdaptedSensorEntry(QString::number(hwSensorId)
    , description, NULL)
    , hwSensorId_(hwSensorId)
    , sensor_(NULL)
    , parentAdaptor_(adaptor)
    , errorCode_(SNoError)
{
    if ( sensorProps )
    {
        sensorProps_ = new ariane_sensor_properties_t;
        memcpy(sensorProps_, sensorProps, sizeof(ariane_sensor_properties_t));
    }
    else
    {
        sensorProps_ = NULL;
    }
}

ArianeAdaptedSensorEntry::~ArianeAdaptedSensorEntry()
{
    if ( sensorProps_ )
    {
        delete sensorProps_;
    }

    if ( buffer_ )
    {
        //delete buffer_;
        qDebug() << "WARNING: Note ring buffer for" << name_ << "not cleaned up...";
    }
}

unsigned ArianeAdaptedSensorEntry::interval()
{
    unsigned interval = 0;
    if ( paramMap_.contains( ARIANE_PARAM_NAME_SAMPLING_RATE) )
    {
        uint32_t rate;
        bool ok = subsystemParameter( ARIANE_PARAM_NAME_SAMPLING_RATE, rate );
        Q_ASSERT( ok );

        QObject::setProperty( ARIANE_PARAM_NAME_SAMPLING_RATE, rate );

        interval = 1000 / rate;
    }
    else
    {
        errorCode_ = SaPropertyDoesNotExist;
    }

    return interval;
}

void ArianeAdaptedSensorEntry::setInterval(unsigned interval)
{
    if ( paramMap_.contains( ARIANE_PARAM_NAME_SAMPLING_RATE) )
    {
        uint32_t rate = 1000 / interval;
        setSubsystemParameter( ARIANE_PARAM_NAME_SAMPLING_RATE, rate );
    }
    else
    {
        errorCode_ = SaPropertyDoesNotExist;
    }
}

bool ArianeAdaptedSensorEntry::setProperty(const char * name, const QVariant & value)
{
    if ( paramMap_.contains( name ) )
    {
        // this is a parameter that is residing in the sensor subsystem (update needs to be propagated)
        Q_ASSERT( value.canConvert<int>() );
        setSubsystemParameter( name, value.toInt() );
    }

    return QObject::setProperty( name, value );
}

QVariant ArianeAdaptedSensorEntry::property(const char * name) const
{
    uint32_t value = 0;
    if ( paramMap_.contains( name ) )
    {
        // this is a parameter that is residing in the sensor subsystem (update needs to be retrieved)
        bool ok = const_cast<ArianeAdaptedSensorEntry*>(this)->subsystemParameter( name, value );
        Q_ASSERT( ok );

        const_cast<ArianeAdaptedSensorEntry*>(this)->QObject::setProperty( name, value );
    }
    else
    {
        bool ok = false;
        value = QObject::property( name ).toInt(&ok);
        Q_ASSERT( ok );
    }

    return QVariant( value );
}

bool ArianeAdaptedSensorEntry::subsystemParameter( const char * paramName, uint32_t& newValue )
{
    Q_ASSERT( paramMap_.contains( paramName ) );

    bool wasOpened = false;
    if ( sensor_ == NULL )
    {
        ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open(parentAdaptor_.dev_,
                                                                                      0,
                                                                                      hwSensorId_,
                                                                                      ARIANE_SENSOR_ACCESS_MODE_MONITOR,
                                                                                      &sensor_);
        if ( accessMode < 0 )
        {
            qDebug() << "ERROR cannot open sensor for reading parameter" << paramName << "from sensor subsystem (open failed)";
            errorCode_ = SaCannotAccessSensor;
            return false;
        }

        Q_ASSERT( sensor_->access_mode == accessMode );

        wasOpened = true;
    }

    if ( sensor_->access_mode != ARIANE_SENSOR_ACCESS_MODE_MONITOR &&
         sensor_->access_mode != ARIANE_SENSOR_ACCESS_MODE_CONTROL )
    {
        qDebug() << "ERROR cannot open sensor for reading parameter" << paramName << "from sensor subsystem (monitor access denied)";
        errorCode_ = SaCannotAccessSensor;
        return false;
    }

    uint32_t parameterId = paramMap_.value( paramName ).paramid;
    int rc = ariane_sensor_get_parameter_value(sensor_, parameterId, &newValue);
    if ( rc != 0 )
    {
        qDebug() << "ERROR cannot read back" << paramName << "from sensor subsystem (get_sensor_parameter failed)";
        if (wasOpened)
        {
            rc = ariane_sensor_close(sensor_);
            Q_ASSERT( rc == 0 );
            sensor_ = NULL;
        }
        errorCode_ = SaCannotAccessSensor;
        return false;
    }

    // TODO deal with SENSOR_PARAM_FLAG_SIGNED flag

    if ( (uint32_t)QObject::property( paramName ).toInt() != newValue )
    {
        const_cast<ArianeAdaptedSensorEntry*>(this)->QObject::setProperty( paramName, QVariant(newValue) );
        emit propertyChanged( paramName );
    }

    if (wasOpened)
    {
        rc = ariane_sensor_close(sensor_);
        Q_ASSERT( rc == 0 );
        sensor_ = NULL;
    }

    errorCode_ = SNoError;
    return true;
}

// TODO: refacoring because it seems (almost) identical to subsystemParameter(...)
bool ArianeAdaptedSensorEntry::setSubsystemParameter( const char * paramName, uint32_t newValue )
{
    Q_ASSERT( paramMap_.contains( paramName ) );

    ariane_sensor_parameter_t parameterDescription = paramMap_.value( paramName );
    if (parameterDescription.flags & ARIANE_SENSOR_PARAM_FLAG_READONLY)
    {
        qWarning() << "WARNING cannot write read-only parameter";
        errorCode_ = SaAccessNotAllowed;
        return false;
    }

    if (parameterDescription.flags & ARIANE_SENSOR_PARAM_FLAG_RANGE)
    {
        if ( newValue < parameterDescription.min || newValue > parameterDescription.max )
        {
            qWarning() << "WARNING new parameter value" << newValue
                       << "for parameter name" << paramName
                       << "not in valid range (" << parameterDescription.min
                       << " <= value <= " << parameterDescription.max
                       << ")";
            errorCode_ = SaValueOutOfRange;
            return false;
        }
    }

    bool wasOpened = false;
    if ( sensor_ == NULL )
    {
        ariane_access_mode_t accessMode = (ariane_access_mode_t)ariane_sensordev_open(parentAdaptor_.dev_,
                                                                                      0,
                                                                                      hwSensorId_,
                                                                                      ARIANE_SENSOR_ACCESS_MODE_CONTROL,
                                                                                      &sensor_);
        if ( accessMode < 0 )
        {
            qWarning() << "WARNING cannot open sensor for writing parameter" << paramName << "to sensor subsystem (open failed)";
            errorCode_ = SaCannotAccessSensor;
            return false;
        }

        Q_ASSERT( sensor_->access_mode == accessMode );

        wasOpened = true;
    }

    if ( sensor_->access_mode != ARIANE_SENSOR_ACCESS_MODE_CONTROL )
    {
        qDebug() << "ERROR cannot open sensor for writing parameter" << paramName << "to subsystem (control access denied)";
        errorCode_ = SaCannotAccessSensor;
        return false;
    }

    // TODO deal with SENSOR_PARAM_FLAG_SIGNED flag here

    ariane_paramid_t parameterId = paramMap_.value( paramName ).paramid;
    int rc = ariane_sensor_set_parameter_value(sensor_, parameterId, &newValue);
    if ( rc != 0 )
    {
        qDebug() << "ERROR cannot write back" << paramName << "to sensor subsystem (set_sensor_parameter failed)";
        if (wasOpened)
        {
            rc = ariane_sensor_close(sensor_);
            Q_ASSERT( rc == 0 );
            sensor_ = NULL;
        }
        errorCode_ = SaCannotAccessSensor;
        return false;
    }

    if ( (uint32_t)QObject::property( paramName ).toInt() != newValue )
    {
        QObject::setProperty( paramName, QVariant(newValue) );
        emit propertyChanged( paramName );
    }

    if (wasOpened)
    {
        rc = ariane_sensor_close(sensor_);
        Q_ASSERT( rc == 0 );
        sensor_ = NULL;
    }

    errorCode_ = SNoError;
    return true;
}

void ArianeAdaptedSensorEntry::print() const
{
    qDebug() << "Sensor Properties:";
    qDebug() << "sensor id        =" << hwSensorId_;
    if ( sensorProps_ )
    {
        qDebug() << "sample rate [Hz] =" << sensorProps_->sample_rate;
        qDebug() << "resolution bits  =" << sensorProps_->resolution_bits;
        qDebug() << "sample channels  =" << sensorProps_->sample_channels;
        qDebug() << "bit flags        =" << sensorProps_->flags;

        Q_ASSERT( sensorProps_->sample_channels <= ARIANE_MAX_CHANNELS );
        for (int i = 0; i < sensorProps_->sample_channels; i++)
        {
            qDebug() << "unit conversion of sample ="
                   << "  scale  =" << sensorProps_->conversions[i].scale
                   << ", offset =" << sensorProps_->conversions[i].offset;
        }
    }
    else
    {
        qDebug() << "no properties present";
    }

    if ( !paramMap_.isEmpty() )
    {
        qDebug() << "Sensor Parameters (" << paramMap_.size() << "):";
        foreach (ariane_sensor_parameter_t paramDescription, paramMap_)
        {
            qDebug() << "param id         =" << paramDescription.paramid;
            qDebug() << "description      = '" << paramDescription.description << "'";

            if ( paramDescription.flags & ARIANE_SENSOR_PARAM_FLAG_RANGE)
            {
                qDebug() << "min value        =" << paramDescription.min;
                qDebug() << "max value        =" << paramDescription.max;
            }
            else
            {
                qDebug() << "Parameter min and max values are not present.";
            }
            qDebug() << "flags            =" << paramDescription.flags
                     << "( PARAM_FLAG_READONLY =" << (bool)(paramDescription.flags & ARIANE_SENSOR_PARAM_FLAG_READONLY)
                     << ", PARAM_FLAG_SIGNED ="   << (bool)(paramDescription.flags & ARIANE_SENSOR_PARAM_FLAG_SIGNED)
                     << " )";

            qDebug() << "value (cached)   =" << QObject::property(paramDescription.description).toInt();
        }
    }
    else
    {
        qDebug() << "No sensor parameters found";
    }
}

void ArianeAdaptedSensorEntry::createRingBuffer()
{
    Q_ASSERT( sensor_ );
    Q_ASSERT( buffer_ == NULL );

    if ( parentAdaptor_.variantMode() )
    {
        buffer_ = new DeviceAdaptorRingBuffer<QVariant>(1024);
    }
    else
    {
        switch ( sensor_->datafmt )
        {
            case ARIANE_EVENT_DATAFMT_XYZ16 :
                //ariane_event_xyz16_t xyz;
                buffer_ = new DeviceAdaptorRingBuffer<TimedXyzData>(1024);
            break;
            case ARIANE_EVENT_DATAFMT_U16 :
                //uint16_t u16;
            case ARIANE_EVENT_DATAFMT_U32 :
                //uint32_t u32;
                buffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(1024);
            break;
            case ARIANE_EVENT_DATAFMT_KEY :
                //ariane_event_key_t key;
                buffer_ = new DeviceAdaptorRingBuffer<TimedKey>(1024);
            break;
            case ARIANE_EVENT_DATAFMT_APPLICATION :
                //uint8_t app[ARIANE_MAX_EVENT_SIZE];
            case ARIANE_EVENT_DATAFMT_ERROR :
                //int16_t error;
            case ARIANE_EVENT_DATAFMT_NONE :
                qDebug() << "WARNING: Detected unsupported sensor data format" << sensor_->datafmt;
            break;

            default :
                qDebug() << "ERROR: Detected unknown sensor data format " << sensor_->datafmt;
                Q_ASSERT( false );
            break;
        }
    }
}

void ArianeAdaptedSensorEntry::destroyRingBuffer()
{
    Q_ASSERT( buffer_ );
    qDebug() << "WARNING: ring buffer not being released" << buffer_;
    //delete buffer_;
    buffer_ = NULL;
}

void ArianeAdaptedSensorEntry::staticEventHandler(const ariane_event_t *event, void* cookie)
{
    //printEvent(event, cookie);

    ArianeAdaptedSensorEntry* entry = static_cast<ArianeAdaptedSensorEntry*>(cookie);
    Q_ASSERT( entry );

    if ( entry->parentAdaptor_.variantMode() )
    {
        entry->eventHandlerVariant(event);
    }
    else
    {
        entry->eventHandler(event);
    }
}

void ArianeAdaptedSensorEntry::printEvent(const ariane_event_t *event, void* cookie)
{
    Q_ASSERT ( event );

    switch (event->datafmt)
    {
        case ARIANE_EVENT_DATAFMT_XYZ16 :
            //ariane_event_xyz16_t xyz;
            qDebug() << "ARIANE_EVENT_DATAFMT_XYZ16" << cookie << event->timestamp << event->data.xyz.x << event->data.xyz.y << event->data.xyz.z;
        break;
        case ARIANE_EVENT_DATAFMT_U16 :
            //uint16_t u16;
            qDebug() << "ARIANE_EVENT_DATAFMT_U16" << cookie << event->timestamp << event->data.u16;
        break;
        case ARIANE_EVENT_DATAFMT_U32 :
            //uint32_t u32;
            qDebug() << "ARIANE_EVENT_DATAFMT_U32" << cookie << event->timestamp << event->data.u32;
        break;
        case ARIANE_EVENT_DATAFMT_KEY :
            qDebug() << "ARIANE_EVENT_DATAFMT_U32" << cookie << event->timestamp
                     << event->data.key.key << event->data.key.pressed << event->data.key.released;
        case ARIANE_EVENT_DATAFMT_APPLICATION :
            //uint8_t app[ARIANE_MAX_EVENT_SIZE];
        case ARIANE_EVENT_DATAFMT_ERROR :
            //int16_t error;
        case ARIANE_EVENT_DATAFMT_NONE :
            Q_ASSERT( false ); // not yet implemented...
        break;
    }
}

void ArianeAdaptedSensorEntry::eventHandler(const ariane_event_t *event)
{
    Q_ASSERT ( event );

    Q_ASSERT( sensor_->datafmt == event->datafmt );
    if ( event->datafmt != ARIANE_EVENT_DATAFMT_ERROR )
    {
        switch (event->datafmt)
        {
            case ARIANE_EVENT_DATAFMT_XYZ16 :
                //ariane_event_xyz16_t xyz;
                parentAdaptor_.handleTimedXyzData(buffer_, event->timestamp, event->data.xyz);
            break;
            case ARIANE_EVENT_DATAFMT_U16 :
                //uint16_t u16;
                parentAdaptor_.handleTimedUnsigned(buffer_, event->timestamp, event->data.u16);
            break;
            case ARIANE_EVENT_DATAFMT_U32 :
                //uint32_t u32;
                parentAdaptor_.handleTimedUnsigned(buffer_, event->timestamp, event->data.u32);
            break;
            case ARIANE_EVENT_DATAFMT_KEY :
                //ariane_event_key_t key;
                parentAdaptor_.handleTimedKey(buffer_, event->timestamp, event->data.key);
            break;
            case ARIANE_EVENT_DATAFMT_APPLICATION :
                //uint8_t app[ARIANE_MAX_EVENT_SIZE];
            case ARIANE_EVENT_DATAFMT_ERROR :
                //int16_t error;
            case ARIANE_EVENT_DATAFMT_NONE :
                qDebug() << "Unimplemented data format: " << event->datafmt;
                Q_ASSERT( false ); // not yet implemented...
            break;
        }
    }
    else
    {
        qWarning() << __PRETTY_FUNCTION__ << "Received asynchronous error:" << event->data.error;
    }
}

void ArianeAdaptedSensorEntry::eventHandlerVariant(const ariane_event_t *event)
{
    Q_ASSERT ( event );

    Q_ASSERT( sensor_->datafmt == event->datafmt );
    if ( event->datafmt != ARIANE_EVENT_DATAFMT_ERROR )
    {
        switch (event->datafmt)
        {
            case ARIANE_EVENT_DATAFMT_XYZ16 :
                //ariane_event_xyz16_t xyz;
                parentAdaptor_.handleTimedXyzDataVariant(buffer_, event->timestamp, event->data.xyz);
            break;
            case ARIANE_EVENT_DATAFMT_U16 :
                //uint16_t u16;
                parentAdaptor_.handleTimedUnsignedVariant(buffer_, event->timestamp, event->data.u16);
            break;
            case ARIANE_EVENT_DATAFMT_U32 :
                //uint32_t u32;
                parentAdaptor_.handleTimedUnsignedVariant(buffer_, event->timestamp, event->data.u32);
            break;
            case ARIANE_EVENT_DATAFMT_KEY :
                //ariane_event_key_t key;
                parentAdaptor_.handleTimedKeyVariant(buffer_, event->timestamp, event->data.key);
            break;
            case ARIANE_EVENT_DATAFMT_APPLICATION :
                //uint8_t app[ARIANE_MAX_EVENT_SIZE];
            case ARIANE_EVENT_DATAFMT_ERROR :
                //int16_t error;
            case ARIANE_EVENT_DATAFMT_NONE :
                qDebug() << "Unimplemented data format: " << event->datafmt;
                Q_ASSERT( false ); // not yet implemented...
            break;
        }
    }
    else
    {
        qWarning() << __PRETTY_FUNCTION__ << "Received asynchronous error:" << event->data.error;
    }
}
