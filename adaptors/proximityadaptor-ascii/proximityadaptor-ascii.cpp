/**
   @file proximityadaptor-ascii.cpp
   @brief ProximityAdaptor that reads proximity value from ascii interface

   <p>
   Copyright (C) 2009-2010 Intel Corporation
   Copyright (C) 2009-2010 Nokia Corporation

   @author Leo Yan <leo.yan@intel.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
   @author Tapio Rantala <ext-tapio.rantala@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#include <errno.h>
#include <string.h>
#include "logging.h"
#include "config.h"
#include "proximityadaptor-ascii.h"
#include "datatypes/utils.h"

#define SYSFS_PROXIMITY_PATH "/sys/bus/i2c/devices/5-0055/apds9802ps/proximity_output"

ProximityAdaptorAscii::ProximityAdaptorAscii(const QString& id) :
                        SysfsAdaptor(id, SysfsAdaptor::IntervalMode)
{
    introduceAvailableDataRange(DataRange(0, 4096, 1));
    addPath(SYSFS_PROXIMITY_PATH);
    proximityBuffer_ = new DeviceAdaptorRingBuffer<ProximityData>(1);
    addAdaptedSensor("proximity", "apds9802ps ascii", proximityBuffer_);
}

ProximityAdaptorAscii::~ProximityAdaptorAscii()
{
    delete proximityBuffer_;
}

void ProximityAdaptorAscii::processSample(int, int fd)
{
    char buf[16];
    lseek(fd, 0, SEEK_SET);
    if (read(fd, buf, sizeof(buf)) <= 0) {
        sensordLogW() << "read(): " << strerror(errno);
        return;
    }
    sensordLogT() << "Proximity output value: " << buf;

    ProximityData* proximity = proximityBuffer_->nextSlot();
    sscanf(buf, "%d", &proximity->value_);
    proximity->withinProximity_ = proximity->value_;
    proximity->timestamp_ = Utils::getTimeStamp();
    proximityBuffer_->commit();
    proximityBuffer_->wakeUpReaders();
}
