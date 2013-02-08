#ifndef OAKTRAILACCELEROMETERADPTOR_H
#define OAKTRAILACCELEROMETERADPTOR_H
#include "sysfsadaptor.h"
#include <QString>
#include <QStringList>
#include <linux/input.h>
#include "deviceadaptorringbuffer.h"
#include "datatypes/orientationdata.h"
#include <QTime>

class OaktrailAccelAdaptor : public SysfsAdaptor {
    Q_OBJECT

    public:
        static DeviceAdaptor* factoryMethod (const QString& id) {
            return new OaktrailAccelAdaptor (id);
        }
        OaktrailAccelAdaptor (const QString& id);
        ~OaktrailAccelAdaptor ();

        bool startSensor ();
        void stopSensor ();

    protected:
        void processSample (int pathId, int fd);

    private:
        DeviceAdaptorRingBuffer<OrientationData>* buffer;
        QString devPath;
        int devId;
};
#endif
