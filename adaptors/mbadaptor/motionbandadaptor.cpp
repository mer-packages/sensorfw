#include "motionbandadaptor.h"

const char* MotionBandAdaptor::MB_DEVICE_NAME = "/dev/rfcomm0";

MotionBandAdaptor::MotionBandAdaptor(const QString& id)
  : DeviceAdaptor(id), deviceFileName_(MB_DEVICE_NAME)
{
    mbDataBuffer_ = new DeviceAdaptorRingBuffer<AccMagGyrData>(BUFFER_SIZE);
    addAdaptedSensor("motionbanddata", "device adaptation of motion band", mbDataBuffer_);

    connect(&timer_, SIGNAL(timeout()), this, SLOT(pollSensorData()));
}

void MotionBandAdaptor::setInterval(unsigned value)
{
    bool active = timer_.isActive();
    if ( active )
    {
        timer_.stop();
    }

    timer_.setInterval(value);

    if ( active )
    {
        timer_.start();
    }
}

bool MotionBandAdaptor::startAdaptor()
{
    bool ok = mbDevice_.sensorOpen(deviceFileName_.toAscii().data());
    if (!ok)
    {
        qDebug() << __PRETTY_FUNCTION__ << "Error opening device:" << deviceFileName_;
        return false;
    }

    int rc = mbDevice_.sensorLoadCaldata(calibrationFileName_.toAscii().data());
    if (rc != 0)
    {
        qDebug() << __PRETTY_FUNCTION__ << "Error loading calibration file:" << calibrationFileName_;
        return false;
    }

    return true;
}

void MotionBandAdaptor::stopAdaptor()
{
    // TODO: sensor should be closed here...
    mbDevice_.sensorClose();
}

bool MotionBandAdaptor::startSensor(const QString& sensorId)
{
    AdaptedSensorEntry* entry = findAdaptedSensor(sensorId);
    if ( entry == NULL )
    {
        qDebug() << __PRETTY_FUNCTION__ << "sensor" << sensorId << "not found";
        return false;
    }

    if (timer_.interval() == 0)
    {
        qDebug() << __PRETTY_FUNCTION__ << "Error: sampling interval not set";
        return false;
    }

    timer_.start();

    entry->setIsRunning( true );

    return true;
}

void MotionBandAdaptor::stopSensor(const QString& sensorId)
{
    AdaptedSensorEntry* entry = findAdaptedSensor(sensorId);
    if ( entry  )
    {
        if ( timer_.isActive() )
        {
            timer_.stop();
        }

        entry->setIsRunning( false );
    }
    else
    {
        qDebug() << __PRETTY_FUNCTION__ << "sensor" << sensorId << "not found";
    }
}

void MotionBandAdaptor::pollSensorData()
{
    sensor_packet* cpkt = NULL;
    do
    {
        cpkt = mbDevice_.sensorPoll();

        if (cpkt)
        {
            //qDebug() << __PRETTY_FUNCTION__ << "Processing motion band packet...";

            //print_packet(spkt);

            AccMagGyrData* data = mbDataBuffer_->nextSlot();
            Q_ASSERT( data );

            // Note: calibration is device specific so it is done in the adaptor (not in generic filters)
            mbDevice_.sensorGetCalibrated(&data->acc, &data->mag, &data->gyr);

            //qDebug() << "OUT: mag" << data->mag.x << data->mag.y << data->mag.z;

            mbDataBuffer_->commit();
            mbDataBuffer_->wakeUpReaders();
        }
        else
        {
            //qDebug() << __PRETTY_FUNCTION__ << "No motion band packet received...";
        }
    }
    while (cpkt != NULL);
}
