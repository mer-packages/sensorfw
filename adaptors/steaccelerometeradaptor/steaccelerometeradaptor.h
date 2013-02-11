#ifndef STEACCELEROMETERADPTOR_H
#define STEACCELEROMETERADPTOR_H
#include "sysfsadaptor.h"
#include <QString>
#include <QStringList>
#include <linux/input.h>
#include "deviceadaptorringbuffer.h"
#include "datatypes/orientationdata.h"
#include <QTime>

class SteAccelAdaptor : public SysfsAdaptor {
    Q_OBJECT

public:
    static DeviceAdaptor* factoryMethod (const QString& id) {
        return new SteAccelAdaptor (id);
    }
    SteAccelAdaptor(const QString& id);
    ~SteAccelAdaptor();

    bool startSensor();
    void stopSensor();

protected:
    void processSample(int pathId, int fd);
    virtual bool setStandbyOverride(const bool override) { Q_UNUSED(override); return false; }

private:
    DeviceAdaptorRingBuffer<OrientationData>* buffer;
    QString devPath;
    int devId;
    QByteArray powerStatePath;
    QByteArray range;
    int frequency;
    bool displayOn;
};
#endif
