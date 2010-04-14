#include "raworientationsensor.h"
#include "sensord/sensormanager.h"
#include "sensord/bin.h"
#include "adaptors/mbadaptor/motionbandadaptor.h"
#include "sensord/bufferreader.h"

RawOrientationSensorChannel::RawOrientationSensorChannel(const QString& id) :
    AbstractSensorChannel(id),
    DbusEmitter<DeviceAxesData>(10)
{
    // set up a filter chain
    filterBin_ = new Bin;
    SensorManager& sm = SensorManager::instance();
    // TODO: device adaptors can not be deleted as long as dependant sensors exist
    motionBand_ = dynamic_cast<MotionBandAdaptor*>(sm.requestDeviceAdaptor("motionbandadaptor"));
    Q_ASSERT( motionBand_ );

    const QString CALIB_FILE_NAME = "mb0.cal";
    motionBand_->setCalibrationFileName(CALIB_FILE_NAME);

    const unsigned SAMPLE_INTERVAL = 40;
    motionBand_->setInterval(SAMPLE_INTERVAL);

    accMagGyrReader_ = new BufferReader<AccMagGyrData>(10);

    headTrackFilter_ = new HeadTrackFilter;

    outputBuffer_ = new RingBuffer<DeviceAxesData>(1024);

    filterBin_->add(accMagGyrReader_, "amgreader");
    filterBin_->add(headTrackFilter_, "converter");
    filterBin_->add(outputBuffer_, "outputbuffer");

    bool joined = filterBin_->join("amgreader", "source", "converter", "sink");
    Q_ASSERT( joined );
    joined = filterBin_->join("converter", "source", "outputbuffer", "sink");
    Q_ASSERT( joined );

    RingBufferBase* rb;
    rb = motionBand_->findBuffer("motionbanddata");
    Q_ASSERT(rb);
    joined = rb->join(accMagGyrReader_);
    Q_ASSERT( joined );

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    joined = outputBuffer_->join(this);
    Q_ASSERT( joined );

    marshallingBin_->start();
    filterBin_->start();

    bool ok = motionBand_->startAdaptor();
    Q_ASSERT( ok );
}

RawOrientationSensorChannel::~RawOrientationSensorChannel()
{
    motionBand_->stopAdaptor();

    filterBin_->stop();
    marshallingBin_->stop();

    SensorManager& sm = SensorManager::instance();
    sm.releaseDeviceAdaptor("motionbandadaptor");

    delete accMagGyrReader_;
    delete headTrackFilter_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

void RawOrientationSensorChannel::start()
{
    AbstractSensorChannel::start();

    marshallingBin_->start();
    filterBin_->start();
    motionBand_->startSensor("motionbanddata");
}

void RawOrientationSensorChannel::stop()
{
    AbstractSensorChannel::stop();

    filterBin_->stop();
    marshallingBin_->stop();
    motionBand_->stopSensor("motionbanddata");
}

void RawOrientationSensorChannel::resetOrientation()
{
    headTrackFilter_->resetOrientation();
}

void RawOrientationSensorChannel::emitToDbus(const DeviceAxesData& value)
{
    /*
    qDebug() << "emitToDbus x" << value.x.x << value.x.y << value.x.z;
    qDebug() << "emitToDbus y" << value.y.x << value.y.y << value.y.z;
    qDebug() << "emitToDbus z" << value.z.x << value.z.y << value.z.z;
    */
    emit dataAvailable(value);
}
