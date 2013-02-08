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
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>

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
#include "datatypes/utils.h"
#include <stdlib.h>
#include <linux/types.h>
#include <string.h>

ALSAdaptorAscii::ALSAdaptorAscii(const QString& id) : SysfsAdaptor(id, SysfsAdaptor::IntervalMode)
{
    memset(buf, 0x0, 16);
    alsBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(1);
    setAdaptedSensor("als", "Internal ambient light sensor lux values", alsBuffer_);
    setDescription("Ambient light");

    // Get range from a file, if the path is found in configuration
    QString rangeFilePath_ = Config::configuration()->value("als/range_file_path",QVariant("")).toString();
    if (rangeFilePath_ != "") {
        QFile sysFile(rangeFilePath_);

        if (!(sysFile.open(QIODevice::ReadOnly))) {
            sensordLogW() << "Unable to config ALS range from sysfs";
        } else {
            sysFile.readLine(buf, sizeof(buf));
            int range = QString(buf).toInt();

            introduceAvailableDataRange(DataRange(0, range, 1));
            sensordLogT() << "Ambient light range: " << range;
        }
    }
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
