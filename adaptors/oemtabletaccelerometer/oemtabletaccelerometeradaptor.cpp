#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "config.h"
#include "oemtabletaccelerometeradaptor.h"
#include "logging.h"
#include "datatypes/utils.h"

OemtabletAccelAdaptor::OemtabletAccelAdaptor (const QString& id) :
    SysfsAdaptor (id, SysfsAdaptor::IntervalMode)
{
    struct stat st;

    devPath = Config::configuration ()->value ("oem_tablet_acc_sys_path").toString ();
    if ( lstat (devPath.toAscii().constData(), &st) < 0 ) {
        sensordLogW () << devPath << "no found";
        return;
    }

    devId = 0;
    addPath (devPath, devId);
    buffer = new DeviceAdaptorRingBuffer<OrientationData>(128);
    setAdaptedSensor("accelerometer", "OEM tablet accelerometer", buffer);

    setDescription("OEM tablet accelerometer");
    introduceAvailableDataRange(DataRange(-256, 256, 1));
    introduceAvailableInterval(DataRange(10, 586, 0));
    setDefaultInterval(0);
}

OemtabletAccelAdaptor::~OemtabletAccelAdaptor () {
    delete buffer;
}

bool OemtabletAccelAdaptor::startSensor () {
    if ( !(SysfsAdaptor::startSensor ()) )
        return false;

    sensordLogD() << "OEM tablet AccelAdaptor start\n";
    return true;
}

void OemtabletAccelAdaptor::stopSensor () {
    SysfsAdaptor::stopSensor();
    sensordLogD() << "OEM tablet AccelAdaptor stop\n";
}

void OemtabletAccelAdaptor::processSample (int pathId, int fd) {
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
