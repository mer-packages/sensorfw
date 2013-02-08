#ifndef OEMTABLETMAGNETOMETERADAPTOR_H
#define OEMTABLETMAGNETOMETERADAPTOR_H

#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/timedunsigned.h"

class OemtabletMagnetometerAdaptor : public SysfsAdaptor
{
    Q_OBJECT;
public:
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new OemtabletMagnetometerAdaptor(id);
    }

protected:
    OemtabletMagnetometerAdaptor(const QString& id);
    ~OemtabletMagnetometerAdaptor();

private:
    void processSample(int pathId, int fd);
    int devId;
    char buf[32];

    DeviceAdaptorRingBuffer<TimedXyzData>* magnetBuffer_;
};

#endif
