#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <QDebug>
#include <QFile>


#include "config.h"
#include "steaccelerometeradaptor.h"
#include "logging.h"
#include "datatypes/utils.h"

#define  GRAVITY_EARTH = 9.812865328

SteAccelAdaptor::SteAccelAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::IntervalMode)
{
    buffer = new DeviceAdaptorRingBuffer<OrientationData>(128);
    setAdaptedSensor("accelerometer", "ste accelerometer", buffer);
    introduceAvailableInterval(DataRange(50, 1000, 0));

/*
range
0: +/- 2g (1 mg/LSB)
1: +/- 4g (2 mg/LSB)
2: +/- 8g (4 mg/LSB)
*/
    QByteArray rangePath = Config::configuration()->value("accelerometer/range_path").toByteArray();
    range = Config::configuration()->value("accelerometer/range_mode").toByteArray();
    if(!rangePath.isEmpty()) {
        writeToFile(rangePath, range);
    }

    /*
power_state/frequency
     0: off
     1: 1 Hz
     2: 10 Hz
     3: 25 Hz
     4: 50 Hz
     5: 100 Hz
     6: 200 Hz
     7: 400 Hz
     */

    powerStatePath = Config::configuration()->value("accelerometer/mode_path").toByteArray();
    frequency = Config::configuration()->value("accelerometer/frequency_mode").toInt();


    int interval;
    switch(frequency) {
    case 0:
        interval = 0; //off
    case 1:
        interval = 1;
        break;
    case 2:
        interval = 10;
        break;
    case 3:
        interval = 25;
        break;
    case 4:
        interval = 50;
        break;
    case 6:
        interval = 200;
        break;
    case 7:
        interval = 400;
        break;
    };

    setDescription("ste accelerometer");
}

SteAccelAdaptor::~SteAccelAdaptor()
{
    delete buffer;
}

bool SteAccelAdaptor::startSensor()
{
    if(!powerStatePath.isEmpty()) {
        writeToFile(powerStatePath, range);
    }

    if ( !(SysfsAdaptor::startSensor()) )
        return false;

    return true;
}

void SteAccelAdaptor::stopSensor()
{
    if(!powerStatePath.isEmpty()) {
        writeToFile(powerStatePath, "0");
    }
    SysfsAdaptor::stopSensor();
}

void SteAccelAdaptor::processSample(int pathId, int fd)
{
    char buf[32];
    int x, y, z;
    qlonglong ts;

//    if (pathId != devId) {
//        sensordLogW () << "Wrong pathId" << pathId;
//        return;
//    }

    lseek(fd, 0, SEEK_SET);
    if (read(fd, buf, sizeof(buf)) < 0 ) {
        sensordLogW() << "Read failed";
        stopSensor();
        return;
    }
    QString line(buf);

    x = line.section(":",0,0).toInt();
    y = line.section(":",1,1).toInt();
    z = line.section(":",2,2).toInt();
    ts = line.section(":",3,3).toLongLong();

    AccelerationData *d = buffer->nextSlot();

    d->timestamp_ = Utils::getTimeStamp();

    d->x_ = x * 0.1 * 9.812865328;
    d->y_ = y * 0.1 * 9.812865328;
    d->z_ = z * 0.1 * 9.812865328;
    buffer->commit();
    buffer->wakeUpReaders();
}
