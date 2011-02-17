#ifndef MRSTACCELEROMETERADPTOR_H
#define MRSTACCELEROMETERADPTOR_H
#include "sysfsadaptor.h"
#include <QString>
#include <QStringList>
#include <linux/input.h>
#include "deviceadaptorringbuffer.h"
#include "datatypes/orientationdata.h"
#include <QTime>

class MRSTAccelAdaptor : public SysfsAdaptor {
    Q_OBJECT

    public:
        static DeviceAdaptor* factoryMethod (const QString& id) {
            return new MRSTAccelAdaptor (id);
        }
        MRSTAccelAdaptor (const QString& id);
        ~MRSTAccelAdaptor ();

        bool startSensor (const QString& sensorid);
        void stopSensor (const QString& sensorid);

    protected:
        void processSample (int pathId, int fd);

    private:
        DeviceAdaptorRingBuffer<OrientationData>* buffer;
        QString devPath;
        int devId;
};
#endif
