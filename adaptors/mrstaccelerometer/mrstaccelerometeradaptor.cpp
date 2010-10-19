#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "config.h"
#include "mrstaccelerometeradaptor.h"
#include "logging.h"
#include "datatypes/utils.h"

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
    buffer = new DeviceAdaptorRingBuffer<OrientationData>(128);
    addAdaptedSensor("accelerometer", "MRST accelerometer", buffer);

    setDescription("MRST accelerometer");
    introduceAvailableDataRange(DataRange(0, 0, 1));
    introduceAvailableInterval(DataRange(0, 0, 0));
    setDefaultInterval(0);
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

    OrientationData* d = buffer->nextSlot ();
    d->timestamp_ = Utils::getTimeStamp();
    d->x_ = x;
    d->y_ = y;
    d->z_ = z;

    buffer->commit ();
    buffer->wakeUpReaders ();
}
