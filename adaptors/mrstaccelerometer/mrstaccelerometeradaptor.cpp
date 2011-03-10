#include "config.h"
#include "mrstaccelerometeradaptor.h"
#include "logging.h"
#include "datatypes/utils.h"

MRSTAccelAdaptor::MRSTAccelAdaptor (const QString& id) :
    SysfsAdaptor (id, SysfsAdaptor::IntervalMode)
{
    addPath (Config::configuration()->value("device_sys_path").toString());
    buffer = new DeviceAdaptorRingBuffer<OrientationData>(1);
    addAdaptedSensor("accelerometer", "MRST accelerometer", buffer);

    setDescription("MRST accelerometer");
    introduceAvailableDataRange(DataRange(0, 0, 1));
    introduceAvailableInterval(DataRange(0, 0, 0));
    setDefaultInterval(0);
}

MRSTAccelAdaptor::~MRSTAccelAdaptor () {
    delete buffer;
}

void MRSTAccelAdaptor::processSample (int pathId, int fd) {
    char buf[32];
    int x, y, z;

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
