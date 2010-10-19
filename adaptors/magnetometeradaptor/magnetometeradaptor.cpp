/**
   @file magnetometeradaptor.cpp
   @brief MagnetometerAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
*/

#include "logging.h"
#include "config.h"
#include "magnetometeradaptor.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include "datatypes/utils.h"
#include <QFile>

/* Device name: /dev/ak8974n, where n is a running number (0 in case on single chip configuration) */
struct ak8974_data {
        __s16 x; /* 0.3uT */
        __s16 y; /* 0.3uT */
        __s16 z; /* 0.3uT */
        __u16 valid;
}; //__attribute__((packed)); <-- documentation states that this is a nogo for c++

MagnetometerAdaptor::MagnetometerAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::IntervalMode),
    originalPollingRate_(1000)
{

    QString driverHandle = getDriverHandle();
    if (driverHandle.size() == 0) {
        sensordLogW() << "Input device not found.";
    } else {
        sensordLogD() << "Detected magnetometer driver at " << driverHandle;
        addPath(driverHandle, 0);
        magnetometerBuffer_ = new DeviceAdaptorRingBuffer<TimedXyzData>(128);
        addAdaptedSensor("magnetometer", "Internal magnetometer coordinates", magnetometerBuffer_);
    }

    // Pick correct datarange based on chip...
    if (driverHandle.contains("8975"))
    {
        introduceAvailableDataRange(DataRange(-4096, 4096, 1));
    } else {
        introduceAvailableDataRange(DataRange(-2048, 2048, 1));
    }

    setDescription("Input device Magnetometer adaptor (ak897x)");
    introduceAvailableInterval(DataRange(1, 2000, 0));
    setDefaultInterval(1000);
}

MagnetometerAdaptor::~MagnetometerAdaptor()
{
    delete magnetometerBuffer_;
}

QString MagnetometerAdaptor::getDriverHandle()
{
    QString magFile = Config::configuration()->value("mag_ak8974_dev_path").toString();
    if (magFile.size() > 0 && QFile::exists(magFile)) {
        return magFile;
    }

    magFile = Config::configuration()->value("mag_ak8975_dev_path").toString();
    if (magFile.size() > 0 && QFile::exists(magFile)) {
        return magFile;
    }

    return "";
}

void MagnetometerAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    struct ak8974_data mag_data;

    unsigned int bytesRead = read(fd, &mag_data, sizeof(mag_data));

    if (bytesRead < sizeof(mag_data)) {
        sensordLogW() << "read" << bytesRead  << "bytes out of expected" << sizeof(mag_data) << "bytes. Previous error:" << strerror(errno);
        //return;
    }

    if (!mag_data.valid) {
        // Can't trust this, printed for curiosity
        sensordLogD() << "Invalid sample received from magnetometer";
    }

    sensordLogT() << "Magnetometer Reading:" << mag_data.x << mag_data.y << mag_data.z;

    TimedXyzData* sample = magnetometerBuffer_->nextSlot();

    sample->timestamp_ = Utils::getTimeStamp();
    sample->x_ = mag_data.x;
    sample->y_ = mag_data.y;
    sample->z_ = mag_data.z;

    magnetometerBuffer_->commit();
    magnetometerBuffer_->wakeUpReaders();

}
