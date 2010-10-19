/**
   @file alsadaptor-ascii.cpp
   @brief ALSAdaptor that reads lux value from ascii interface

   <p>
   Copyright (C) 2009-2010 Intel Corporation
   Copyright (C) 2009-2010 Nokia Corporation

   @author Leo Yan <leo.yan@intel.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
   @author Tapio Rantala <ext-tapio.rantala@nokia.com>

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

#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <QFile>

#include "logging.h"
#include "config.h"
#include "alsadaptor-ascii.h"
#include "filters/utils.h"
#include <stdlib.h>
#include <linux/types.h>

#define SYSFS_RANGE_PATH "/sys/bus/i2c/devices/5-0029/apds9802als/sensing_range"
#define SYSFS_LUX_PATH "/sys/bus/i2c/devices/5-0029/apds9802als/lux_output"
#define DEFAULT_RANGE 65535

ALSAdaptorAscii::ALSAdaptorAscii(const QString& id) : SysfsAdaptor(id, SysfsAdaptor::IntervalMode)
{
    int range = DEFAULT_RANGE;
    QFile sysFile;
    QString devPath;

    // Check if a file has been provided as range source
    sysFile.setFileName(Config::configuration()->value("als-ascii_range_sysfs_path").toString());

    if (sysFile.size() > 0)
    {
        if (!(sysFile.exists() && sysFile.open(QIODevice::ReadOnly))) {
            sensordLogW() << "Unable to config ALS range from sysfs, using default value: " << DEFAULT_RANGE;
        } else {
            sysFile.readLine(buf, sizeof(buf));
            range = QString(buf).toInt();
        }
    }

    sensordLogT() << "Ambient light range: " << range;


    // Locate the actual handle
    devPath = Config::configuration()->value("als-ascii_sysfs_path").toString();

    if (devPath.isEmpty())
    {
        sensordLogW() << "No driver handle found for ALS. Data not available.";
        return;
    }

    addPath(devPath);
    alsBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(16);
    addAdaptedSensor("als", "Internal ambient light sensor lux values", alsBuffer_);

    setDescription("Ambient light");
    introduceAvailableDataRange(DataRange(0, range, 1));
    introduceAvailableInterval(DataRange(50, 2000, 0));
    setDefaultInterval(1000);
}

ALSAdaptorAscii::~ALSAdaptorAscii()
{
    delete alsBuffer_;
}

void ALSAdaptorAscii::processSample(int pathId, int fd) {
    Q_UNUSED(pathId);

    if (read(fd, buf, sizeof(buf)) <= 0) {
        sensordLogW() << "read():" << strerror(errno);
        return;
    }
    buf[sizeof(buf)-1] = '\0';

    sensordLogT() << "Ambient light value: " << buf;

    __u16 idata = atoi(buf);

    TimedUnsigned* lux = alsBuffer_->nextSlot();

    lux->value_ = idata;
    lux->timestamp_ = Utils::getTimeStamp();

    alsBuffer_->commit();
    alsBuffer_->wakeUpReaders();
}
