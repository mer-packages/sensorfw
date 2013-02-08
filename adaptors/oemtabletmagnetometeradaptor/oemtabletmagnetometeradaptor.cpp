#include <errno.h>
#include "logging.h"
#include "config.h"
#include "oemtabletmagnetometeradaptor.h"
#include "datatypes/utils.h"

#define SYSFS_MAGNET_PATH "/sys/bus/i2c/drivers/ak8974/2-000e/ak8974/curr_pos"

OemtabletMagnetometerAdaptor::OemtabletMagnetometerAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::IntervalMode),
    devId(0)
{
    if (access(SYSFS_MAGNET_PATH, R_OK) < 0) {
        sensordLogW() << SYSFS_MAGNET_PATH << ": "<< strerror(errno);
        return;
    }
    addPath(SYSFS_MAGNET_PATH, devId);
    magnetBuffer_ = new DeviceAdaptorRingBuffer<TimedXyzData>(16);
    addAdaptedSensor("magnetometer", "ak8974 ascii", magnetBuffer_);

    setDescription("OEM tablet magnetometer");
    introduceAvailableDataRange(DataRange(-2048, 2048, 1));
    introduceAvailableInterval(DataRange(10, 556, 0));
    setDefaultInterval(500);
}

OemtabletMagnetometerAdaptor::~OemtabletMagnetometerAdaptor()
{
    delete magnetBuffer_;
}

void OemtabletMagnetometerAdaptor::processSample(int pathId, int fd)
{
    int x, y, z;

    if (pathId != devId) {
        sensordLogW() << "pathId != devId";
        return;
    }
    lseek(fd, 0, SEEK_SET);
    if (read(fd, buf, sizeof(buf)) <= 0) {
        sensordLogW() << "read():" << strerror(errno);
        return;
    }
    sensordLogT() << "Magnetometer output value: " << buf;

    sscanf(buf, "(%d,%d,%d)", &x, &y, &z);

    TimedXyzData* pos = magnetBuffer_->nextSlot();
    pos->x_ = x;
    pos->y_ = y;
    pos->z_ = z;
    pos->timestamp_ = Utils::getTimeStamp();

    magnetBuffer_->commit();
    magnetBuffer_->wakeUpReaders();
}
