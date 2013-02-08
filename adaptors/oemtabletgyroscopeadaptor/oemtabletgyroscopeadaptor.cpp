#include <errno.h>

#include "logging.h"
#include "config.h"
#include "datatypes/utils.h"

#include "oemtabletgyroscopeadaptor.h"

OEMTabletGyroscopeAdaptor::OEMTabletGyroscopeAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::IntervalMode)
{
    gyroscopeBuffer_ = new DeviceAdaptorRingBuffer<TimedXyzData>(32);

    setAdaptedSensor("gyroscope", "mpu3050", gyroscopeBuffer_);

    introduceAvailableDataRange(DataRange(-32768, 32767, 1));
    introduceAvailableInterval(DataRange(10, 113, 0));
    setDefaultInterval(100);
}

OEMTabletGyroscopeAdaptor::~OEMTabletGyroscopeAdaptor()
{
    delete gyroscopeBuffer_;
}

void OEMTabletGyroscopeAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);
    short x, y, z;
    char buf[32];

    if (read(fd, buf, sizeof(buf)) <= 0) {
        sensordLogW() << "read():" << strerror(errno);
        return;
    }
    sensordLogT() << "gyroscope output value: " << buf;

    sscanf(buf, "%hd %hd %hd\n", &x, &y, &z);

    TimedXyzData* pos = gyroscopeBuffer_->nextSlot();
    pos->x_ = x;
    pos->y_ = y;
    pos->z_ = z;
    pos->timestamp_ = Utils::getTimeStamp();

    gyroscopeBuffer_->commit();
    gyroscopeBuffer_->wakeUpReaders();
}
