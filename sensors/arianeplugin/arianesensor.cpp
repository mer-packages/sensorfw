#include "sensord/bin.h"
#include "sensord/sensormanager.h"
#include "sensord/parameterparser.h"

#include "arianesensor.h"
#include "arianesensor_a.h"

AbstractSensorChannel* ArianeSensorChannel::factoryMethod(const QString& id)
{
    ArianeSensorChannel* sc = new ArianeSensorChannel(id);
#ifndef NO_DBUS_FOR_ARIANE
    new ArianeSensorChannelAdaptor(sc);
    qDebug() << __PRETTY_FUNCTION__ << "Attached to D-BUS...";
#else
    qDebug() << __PRETTY_FUNCTION__ << "Skipping D-BUS adaptor attachment...";
#endif
    return sc;
}

ArianeSensorChannel::ArianeSensorChannel(const QString& id) :
    AbstractSensorChannel(id),
    DbusEmitter<QVariant>(10),
    hwSensorId_(-1),
    marshallingBin_(NULL),
    ariane_(NULL)
{
    QMap<QString, QString> propMap = ParameterParser::getPropertyMap(id);
    ParameterParser::applyPropertyMap(this, propMap);

    SensorManager& sm = SensorManager::instance();
    if ( !adaptor_.isEmpty() )
    {
        ariane_ = dynamic_cast<ArianeAdaptor*>(sm.requestDeviceAdaptor(adaptor_));
        if ( ariane_ )
        {
            ariane_->setVariantMode( true );

            isValid_ = ariane_->isRunning();
            if (!isValid_)
            {
                setError( SAdaptorNotStarted, QString(tr("could not start device adaptor; %1")).arg(adaptor_) );
            }
        }
        else
        {
            setError(SAdaptorNotObtained, QString(tr("could not obtain requested device adaptor; %1")).arg(adaptor_) );
        }
    }
    else
    {
        setError( SAdaptorNotStarted, tr("device adaptor property not specified") );
    }
}

ArianeSensorChannel::~ArianeSensorChannel()
{
    if ( ariane_ )
    {
        SensorManager& sm = SensorManager::instance();
        sm.releaseDeviceAdaptor(adaptor_);
    }
}

bool ArianeSensorChannel::checkPreconditionValid()
{
    if ( !isValid_ )
    {
        setError(SSensorNotValid, tr("sensor is invalid (construction failed)"));
        return false;
    }

    if ( ariane_ == NULL )
    {
        setError(SAdaptorNotInitialized, tr("device adaptor not initialized"));
        return false;
    }

    if ( hwSensorId_ < 0 )
    {
        setError(SHardwareIdNotInitialized, tr("hardware sensor ID not initialized"));
        return false;
    }

    return true;
}

int ArianeSensorChannel::interval()
{
    return adaptorProperty( "interval" );
}

void ArianeSensorChannel::setInterval(int interval)
{
    setAdaptorProperty("interval", interval );
}

bool ArianeSensorChannel::setAdaptorProperty ( const QString& name, unsigned value )
{
    clearError();

    if ( !checkPreconditionValid() )
    {
        return false;
    }

    bool result = false;
    ArianeAdaptedSensorEntry* entry = dynamic_cast<ArianeAdaptedSensorEntry*>(ariane_->findAdaptedSensor(QString::number(hwSensorId_)));
    if ( entry )
    {
        result = entry->setProperty( name.toAscii().data(), value );
        if ( entry->errorCode() != SNoError )
        {
            setError(entry->errorCode(), QString(tr("can not set adaptor property '%1' for HW sensor '%2'").arg(name).arg(entry->name())));
        }
    }
    else
    {
        // Make sure to initialize the hwSensorId property before setting adaptor entry properties
        setError(SCannotAccessHwSensor, QString(tr("could not find adapted sensor id '%1'").arg(hwSensorId_)) );
    }

    return result;
}

unsigned ArianeSensorChannel::adaptorProperty ( const QString& name )
{
    clearError();

    if ( !checkPreconditionValid() )
        return 0;

    unsigned value = 0;
    ArianeAdaptedSensorEntry* entry = dynamic_cast<ArianeAdaptedSensorEntry*>(ariane_->findAdaptedSensor(QString::number(hwSensorId_)));
    if ( entry )
    {
        QVariant data = entry->property( name.toAscii().data() );
        if ( entry->errorCode() != SNoError )
        {
            setError(entry->errorCode(), QString(tr("can not get adaptor property '%1' for HW sensor '%2'").arg(name).arg(entry->name())));
        }

        bool ok = false;
        value = data.toInt(&ok);
        Q_ASSERT(ok);
    }
    else
    {
        setError(SCannotAccessHwSensor, QString(tr("could not find adapted sensor id '%1'").arg(hwSensorId_)) );
    }

    return value;
}

bool ArianeSensorChannel::start()
{
    clearError();

    if ( !checkPreconditionValid() )
        return false;

    bool ok = ariane_->startSensor( QString::number( hwSensorId_ ) );
    if ( ok )
    {
        RingBufferBase* rb = ariane_->findBuffer(QString::number( hwSensorId_ ));
        if ( rb == NULL )
        {
            setError(SRingBufferNotFound, tr("could not locate ring buffer") );
            isValid_ = false;
            return false;
        }

        marshallingBin_ = new Bin;
        Q_CHECK_PTR( marshallingBin_ );
        marshallingBin_->add(this, "sensorchannel");

        bool joined = rb->join(this);
        if ( !joined )
        {
            setError(SJoinFailed, tr("could not join ring buffer and sensor channel" ));
            isValid_ = false;
            return false;
        }

        marshallingBin_->start();
    }
    else
    {
        setError(SHwSensorStartFailed, tr("could not start hadware sensor" ));
        isValid_ = false;
        return false;
    }
    return true;
}

bool ArianeSensorChannel::stop()
{
    clearError();

    if ( !checkPreconditionValid() )
        return false;

    // TODO: each of these following calls can fail and this is not handled...
    ariane_->stopSensor(QString::number(hwSensorId_));

    Q_ASSERT( marshallingBin_ );
    marshallingBin_->stop();

    delete marshallingBin_;
    return true;
}

void ArianeSensorChannel::emitToDbus(const QVariant& value)
{
    if ( value.canConvert<TimedXyzData>() )
    {
        XYZ xyzData(value.value<TimedXyzData>());
        emit dataAvailable(xyzData);
    }
    else if ( value.canConvert<TimedUnsigned>() )
    {
        Unsigned unsignedData(value.value<TimedUnsigned>());
        emit dataAvailable(unsignedData);
    }
    else if ( value.canConvert<TimedKey>() )
    {
        Key keyData(value.value<TimedKey>());
        emit dataAvailable(keyData);
    }
    else
    {
        qDebug() << __PRETTY_FUNCTION__ << "variant contains unexpected data...";
    }
}
