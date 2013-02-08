#ifndef OEMTABLETALSADAPTOR_ASCII_H
#define OEMTABLETALSADAPTOR_ASCII_H

#include <QObject>
#include <QString>
#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/timedunsigned.h"

class OEMTabletALSAdaptorAscii : public SysfsAdaptor
{
    Q_OBJECT;
public:
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new OEMTabletALSAdaptorAscii(id);
    }

protected:
    OEMTabletALSAdaptorAscii(const QString& id);
    ~OEMTabletALSAdaptorAscii();

    virtual bool setStandbyOverride(const bool override) { Q_UNUSED(override); return false; }
private:

    void processSample(int pathId, int fd);
    char buf[16];

    DeviceAdaptorRingBuffer<TimedUnsigned>* alsBuffer_;
};

#endif
