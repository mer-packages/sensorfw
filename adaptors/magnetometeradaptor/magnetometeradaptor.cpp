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
    SysfsAdaptor(id, SysfsAdaptor::IntervalMode, false)
{
    intervalCompensation_ = Config::configuration()->value<int>("magnetometer/interval_compensation", 0);
    magnetometerBuffer_ = new DeviceAdaptorRingBuffer<TimedXyzData>(1);
    setAdaptedSensor("magnetometer", "Internal magnetometer coordinates", magnetometerBuffer_);
    setOverflowLimit(Config::configuration()->value<int>("magnetometer/overflow_limit", 8000));
    setDescription("Input device Magnetometer adaptor (ak897x)");
}

MagnetometerAdaptor::~MagnetometerAdaptor()
{
    delete magnetometerBuffer_;
}

void MagnetometerAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    struct ak8974_data mag_data;

    unsigned int bytesRead = read(fd, &mag_data, sizeof(mag_data));

    if (bytesRead < sizeof(mag_data)) {
        sensordLogW() << "read " << bytesRead  << " bytes out of expected " << sizeof(mag_data) << " bytes. Previous error: " << strerror(errno);
        //return;
    }

    if (!mag_data.valid) {
        // Can't trust this, printed for curiosity
        sensordLogD() << "Invalid sample received from magnetometer";
    }

    sensordLogT() << "Magnetometer reading: " << mag_data.x << ", " << mag_data.y << ", " << mag_data.z;

    TimedXyzData* sample = magnetometerBuffer_->nextSlot();

    sample->timestamp_ = Utils::getTimeStamp();
    sample->x_ = mag_data.x;
    sample->y_ = mag_data.y;
    sample->z_ = mag_data.z;

    magnetometerBuffer_->commit();
    magnetometerBuffer_->wakeUpReaders();
}

bool MagnetometerAdaptor::setInterval(const unsigned int value, const int sessionId)
{
    if(intervalCompensation_)
    {
        return SysfsAdaptor::setInterval(value > intervalCompensation_ ? value - intervalCompensation_ : 0, sessionId);
    }
    return SysfsAdaptor::setInterval(value, sessionId);
}

void MagnetometerAdaptor::setOverflowLimit(int limit)
{
    setProperty("overflow_limit", limit);
}
