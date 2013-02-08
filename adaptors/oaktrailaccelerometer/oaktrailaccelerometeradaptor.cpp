#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "config.h"
#include "oaktrailaccelerometeradaptor.h"
#include "logging.h"
#include "datatypes/utils.h"

OaktrailAccelAdaptor::OaktrailAccelAdaptor (const QString& id) :
    SysfsAdaptor (id, SysfsAdaptor::IntervalMode)
{
    struct stat st;

    devPath = Config::configuration ()->value ("oaktrail_acc_sys_path").toString ();
    if ( lstat (devPath.toAscii().constData(), &st) < 0 ) {
        sensordLogW () << devPath << "no found";
        return;
    }

    devId = 0;
    addPath (devPath, devId);
    buffer = new DeviceAdaptorRingBuffer<OrientationData>(128);
    setAdaptedSensor("accelerometer", "Oaktrail accelerometer", buffer);

    setDescription("Oaktrail accelerometer");
    introduceAvailableDataRange(DataRange(-256, 256, 1));
    introduceAvailableInterval(DataRange(10, 586, 0));
    setDefaultInterval(100);
}

OaktrailAccelAdaptor::~OaktrailAccelAdaptor () {
    delete buffer;
}

bool OaktrailAccelAdaptor::startSensor () {
    if ( !(SysfsAdaptor::startSensor ()) )
        return false;

    sensordLogD() << "Oaktrail AccelAdaptor start\n";
    return true;
}

void OaktrailAccelAdaptor::stopSensor () {
    SysfsAdaptor::stopSensor();
    sensordLogD() << "Oaktrail AccelAdaptor stop\n";
}

void OaktrailAccelAdaptor::processSample (int pathId, int fd) {
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
