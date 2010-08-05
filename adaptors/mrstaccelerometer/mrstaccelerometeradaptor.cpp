#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "mrstaccelerometeradaptor.h"
#include "logging.h"
#include "filters/utils.h"

MRSTAccelAdaptor::MRSTAccelAdaptor (const QString& id) :
    SysfsAdaptor (id, SysfsAdaptor::IntervalMode)
{
    struct stat st;

    devPath = Config::configuration ()->value ("device_sys_path").toString ();
    if ( lstat (devPath.toAscii().constData(), &st) < 0 ) {
        sensordLogW () << devPath << "no found";
        return;
    }

    devId = 0;
    addPath (devPath, devId);
    buffer = new DeviceAdaptorRingBuffer<OrientationData>(1024);
    addAdaptedSensor("accelerometer", "MRST accelerometer", buffer);
}

MRSTAccelAdaptor::~MRSTAccelAdaptor () {
    delete buffer;
}

bool MRSTAccelAdaptor::startSensor (const QString& sensorid) {
    if ( !(SysfsAdaptor::startSensor (sensorid)) )
        return false;

    sensordLogD() << "MRSTAccelAdaptor start\n";
    return true;
}

void MRSTAccelAdaptor::stopSensor (const QString& sensorid) {
    SysfsAdaptor::stopSensor(sensorid);
    sensordLogD() << "MRSTAccelAdaptor stop\n";
}

void MRSTAccelAdaptor::processSample (int pathId, int fd) {
    char buf[32];
    int x, y, z;

    if ( pathId != devId ) {
        sensordLogW () << "Wrong pathId" << pathId;
        return;
    }

    lseek (fd, 0, SEEK_SET);
    if ( read (fd, buf, sizeof(buf)) < 0 ) {
        sensordLogW () << "Read failed";
        return;
    }

    if ( sscanf (buf, "(%d,%d,%d)", &x, &y, &z) == 0 ) {
        sensordLogW () << "Wrong data format";
        return;
    }

    static float step = 2.0 / 116.0;
    float fx, fy, fz;

    /* calibration */
    x -= 1;
    y += 1;

    fx = x * step;
    fy = y * step;
    fz = z * step;

    /* Translate to mg */
    x = fx * 1000;
    y = fy * 1000;
    z = fz * 1000;

    OrientationData* d = buffer->nextSlot ();
    d->timestamp_ = Utils::getTimeStamp();
    d->x_ = x;
    d->y_ = y;
    d->z_ = z;

    buffer->commit ();
    buffer->wakeUpReaders ();
}
