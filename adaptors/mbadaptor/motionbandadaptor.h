#ifndef MOTIONBANDADAPTOR_H
#define MOTIONBANDADAPTOR_H

#include <QTimer>

#include "sensord/deviceadaptor.h"
#include "sensord/deviceadaptorringbuffer.h"
#include "libmb/reader.h"
#include "filters/motionbanddata.h"

class MotionBandAdaptor : public DeviceAdaptor
{
    Q_OBJECT

    Q_PROPERTY(unsigned interval READ interval WRITE setInterval)
    Q_PROPERTY(QString deviceFileName READ deviceFileName WRITE setDeviceFileName)
    Q_PROPERTY(QString calibrationFileName READ calibrationFileName WRITE setCalibrationFileName)

public:
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new MotionBandAdaptor(id);
    }

    unsigned interval() const { return timer_.interval(); }
    void setInterval(unsigned value);

    const QString& deviceFileName() const { return deviceFileName_; }
    void setDeviceFileName(const QString& fileName) { deviceFileName_ = fileName; }

    const QString& calibrationFileName() const { return calibrationFileName_; }
    void setCalibrationFileName(const QString& fileName) { calibrationFileName_ = fileName; }

    bool startAdaptor();
    void stopAdaptor();

    virtual bool startSensor(const QString& sensorId);
    virtual void stopSensor(const QString& sensorId);

    static const unsigned BUFFER_SIZE = 1;

protected:
    MotionBandAdaptor(const QString& id);

private slots:
    void pollSensorData();

private:
    static const char* MB_DEVICE_NAME;

    QString deviceFileName_;
    QString calibrationFileName_;

    DeviceAdaptorRingBuffer<AccMagGyrData>* mbDataBuffer_;

    MbDevice mbDevice_;
    QTimer   timer_;
};

#endif // MOTIONBANDADAPTOR_H
