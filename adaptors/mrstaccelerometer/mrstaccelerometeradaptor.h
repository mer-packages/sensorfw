#ifndef MRSTACCELEROMETERADPTOR_H
#define MRSTACCELEROMETERADPTOR_H

#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/orientationdata.h"

class MRSTAccelAdaptor : public SysfsAdaptor
{
    Q_OBJECT

public:
    static DeviceAdaptor* factoryMethod (const QString& id) {
        return new MRSTAccelAdaptor (id);
    }

    MRSTAccelAdaptor (const QString& id);
    ~MRSTAccelAdaptor ();

protected:
    void processSample (int pathId, int fd);

private:
    DeviceAdaptorRingBuffer<OrientationData>* buffer;
};
#endif
