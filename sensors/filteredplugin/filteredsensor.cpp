#include "sensord/sensormanager.h"
#include "sensord/bin.h"
#include "sensord/bufferreader.h"
#include "datatypes/orientation.h"
#include "filters/orientationconverter.h"

#include "filteredsensor.h"
#include "filteredsensor_a.h"

// TODO this should not be hardwired
const QString FilteredSensorChannel::ORIENTATION_HW_SENSOR_NAME = "2064";

AbstractSensorChannel* FilteredSensorChannel::factoryMethod(const QString& id)
{
    FilteredSensorChannel* sc = new FilteredSensorChannel(id);
#ifndef NO_DBUS_FOR_ARIANE
    new FilteredSensorChannelAdaptor(sc);
    qDebug() << __PRETTY_FUNCTION__ << "Attached to D-BUS...";
#else
    qDebug() << __PRETTY_FUNCTION__ << "Skipping D-BUS adaptor attachment...";
#endif
    return sc;
}

FilteredSensorChannel::FilteredSensorChannel(const QString& id) :
    AbstractSensorChannel(id),
    DbusEmitter<OrientationData>(10),
    filterBin_(NULL),
    marshallingBin_(NULL),
    ariane_(NULL),
    orientationReader_(NULL),
    orientationConvertor_(NULL),
    outputBuffer_(NULL)
{
    QMap<QString, QString> propMap = ParameterParser::getPropertyMap(id);
    ParameterParser::applyPropertyMap(this, propMap);

    SensorManager& sm = SensorManager::instance();
    if ( !adaptor_.isEmpty() )
    {
        ariane_ = dynamic_cast<ArianeAdaptor*>(sm.requestDeviceAdaptor(adaptor_));
        if ( ariane_ )
        {
            isValid_ = ariane_->isRunning();
            if (!isValid_)
            {
                setError(SAdaptorNotStarted, tr("could not start device adaptor") + adaptor_);
            }
        }
        else
        {
            setError(SAdaptorNotObtained, tr("could not obtain requested device adaptor ") + adaptor_);
        }
    }
    else
    {
        setError( SAdaptorNotStarted, tr("device adaptor property not specified") );
    }
}

FilteredSensorChannel::~FilteredSensorChannel()
{
    if ( ariane_ )
    {
        SensorManager& sm = SensorManager::instance();
        sm.releaseDeviceAdaptor(adaptor_);
    }
}

bool FilteredSensorChannel::checkPreconditionValid()
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

    return true;
}

int FilteredSensorChannel::interval()
{
    return adaptorProperty( "interval" );
}

void FilteredSensorChannel::setInterval(int interval)
{
    setAdaptorProperty("interval", interval );
}

bool FilteredSensorChannel::setAdaptorProperty ( const QString& name, unsigned value )
{
    clearError();

    if ( !checkPreconditionValid() )
        return 0;


    bool result = false;
    ArianeAdaptedSensorEntry* entry = dynamic_cast<ArianeAdaptedSensorEntry*>(ariane_->findAdaptedSensor(ORIENTATION_HW_SENSOR_NAME));
    if ( entry )
    {
        result = entry->setProperty( name.toAscii().data(), value );
        if ( entry->errorCode() != SNoError )
        {
            setError(entry->errorCode(), tr("can not set adaptor property '") + name + tr("' for sensor: ") + entry->name());
        }
    }
    else
    {
        // Make sure to initialize the hwSensorId property before setting adaptor entry properties
        setError(SCannotAccessHwSensor, tr("could not find adapted sensor id ") + ORIENTATION_HW_SENSOR_NAME );
    }

    return result;
}

unsigned FilteredSensorChannel::adaptorProperty ( const QString& name )
{
    clearError();

    if ( !checkPreconditionValid() )
        return 0;

    unsigned value = 0;
    ArianeAdaptedSensorEntry* entry = dynamic_cast<ArianeAdaptedSensorEntry*>(ariane_->findAdaptedSensor(ORIENTATION_HW_SENSOR_NAME));
    if ( entry )
    {
        QVariant data = entry->property( name.toAscii().data() );
        if ( entry->errorCode() != SNoError )
        {
            setError(entry->errorCode(), tr("can not get adaptor property '") + name + tr("' for sensor: ") + entry->name());
        }

        bool ok = false;
        value = data.toInt(&ok);
        Q_ASSERT(ok);
    }
    else
    {
        setError(SPropertyGetError, tr("can not get adaptor ") + ORIENTATION_HW_SENSOR_NAME);
    }

    return value;
}

bool FilteredSensorChannel::start()
{
    clearError();

    if ( !checkPreconditionValid() )
        return false;

    orientationReader_ = new BufferReader<TimedXyzData>(10);

    orientationConvertor_ = new OrientationConverter;
    Q_CHECK_PTR( orientationConvertor_ );

    outputBuffer_ = new RingBuffer<OrientationData>(1024);
    Q_CHECK_PTR( outputBuffer_ );

    filterBin_ = new Bin;
    Q_CHECK_PTR( filterBin_ );

    filterBin_->add(orientationReader_, "orientation");
    filterBin_->add(orientationConvertor_, "converter");
    filterBin_->add(outputBuffer_, "outputbuffer");

    bool joined = filterBin_->join("orientation", "source", "converter", "sink");
    if ( !joined )
    {
        setError(SJoinFailed, tr("could not join orientation and convertor" ));
        isValid_ = false;
        return false;
    }

    joined = filterBin_->join("converter", "source", "outputbuffer", "sink");
    if ( !joined )
    {
        setError(SJoinFailed, tr("could not join convertor and output buffer" ));
        isValid_ = false;
        return false;
    }

    // TODO: start should come after chain setup
    //Q_ASSERT( interval_ > 0 ); // TODO: instead of interval the "Sample rate" adaptor property should be used...
    bool ok = ariane_->startSensor(ORIENTATION_HW_SENSOR_NAME);
    if (ok)
    {

        RingBufferBase* rb;
        rb = ariane_->findBuffer(ORIENTATION_HW_SENSOR_NAME);
        if ( rb == NULL )
        {
            setError(SRingBufferNotFound, tr("could not locate ring buffer") );
            isValid_ = false;
            return false;
        }

        joined = rb->join(orientationReader_);
        if ( !joined )
        {
            setError(SJoinFailed, tr("could not join ring buffer and orientation reader" ));
            isValid_ = false;
            return false;
        }

        marshallingBin_ = new Bin;
        Q_CHECK_PTR( marshallingBin_ );
        marshallingBin_->add(this, "sensorchannel");

        joined = outputBuffer_->join(this);
        if ( !joined )
        {
            setError(SJoinFailed, tr("could not join output buffer and sensor channel" ));
            isValid_ = false;
            return false;
        }

        marshallingBin_->start();
        filterBin_->start();

        //Q_ASSERT( interval_ > 0 ); // TODO: instead of interval the "Sample rate" adaptor property should be used...
        //ariane_->startSensor(ORIENTATION_HW_SENSOR_NAME);
    }
    else
    {
        setError(SHwSensorStartFailed, tr("could not start hadware sensor" ));
        isValid_ = false;
        return false;
    }

    return true;
}

bool FilteredSensorChannel::stop()
{
    clearError();

    if ( !checkPreconditionValid() )
        return false;

    if (ariane_)
    {
        ariane_->stopSensor(ORIENTATION_HW_SENSOR_NAME);
    }

    if (filterBin_) filterBin_->stop();
    if (marshallingBin_) marshallingBin_->stop();

    if (orientationReader_) delete orientationReader_;
    if (orientationConvertor_) delete orientationConvertor_;
    if (outputBuffer_) delete outputBuffer_;
    if (marshallingBin_) delete marshallingBin_;
    if (filterBin_) delete filterBin_;

    return true;
}

void FilteredSensorChannel::emitToDbus(const OrientationData& value)
{
    emit dataAvailable(value);
}
