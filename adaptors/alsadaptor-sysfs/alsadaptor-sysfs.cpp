/**
   @file alsadaptor-sysfs.cpp
   @brief ALSAdaptor that reads lux values from a sysfs node

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
   @author Tapio Rantala <ext-tapio.rantala@nokia.com>
   @author Markus Lehtonen <markus.lehtonen@nokia.com>

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
#include "alsadaptor-sysfs.h"
#include <errno.h>
#include <stdlib.h>
#include "datatypes/utils.h"
#include <linux/types.h>

ALSAdaptorSysfs::ALSAdaptorSysfs(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::IntervalMode, true, Config::configuration()->value("als-sysfs_path").toString())
{
    alsBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(1024);
    addAdaptedSensor("als", "Internal ambient light sensor lux values", alsBuffer_);

    introduceAvailableDataRange(DataRange(0, 65535, 1));
    introduceAvailableInterval(DataRange(0, 0, 0));
    setDefaultInterval(0);
}

ALSAdaptorSysfs::~ALSAdaptorSysfs()
{
    delete alsBuffer_;
}

void ALSAdaptorSysfs::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    char asciidata[6];
    __u16 idata = 0;

    int bytesRead = read(fd, &asciidata, sizeof(asciidata));
    asciidata[sizeof(asciidata)-1] = '\0';

    idata = atoi(asciidata);

    if (bytesRead <= 0) {
        sensordLogW() << "read():" << strerror(errno);
        return;
    }

    sensordLogT() << "Ambient light value: " << idata;

    TimedUnsigned* lux = alsBuffer_->nextSlot();
    lux->value_ = idata;

    lux->timestamp_ = Utils::getTimeStamp();

    alsBuffer_->commit();
    alsBuffer_->wakeUpReaders();
}
