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

#include "logging.h"
#include "config.h"
#include "stealsadaptor-sysfs.h"
#include <errno.h>
#include <stdlib.h>
#include "datatypes/utils.h"
#include <linux/types.h>
#include <QDebug>

SteAlsAdaptorSysfs::SteAlsAdaptorSysfs(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::IntervalMode)
{
    sensordLogC() << Q_FUNC_INFO;

    alsBuffer = new DeviceAdaptorRingBuffer<TimedUnsigned>(1);
    setAdaptedSensor("als", "Internal ambient light sensor lux values", alsBuffer);
    setDescription("Ambient light");

    powerStatePath = Config::configuration()->value<QByteArray>("als/powerstate_path", "");
    powerMode = Config::configuration()->value<QByteArray>("als/mode","");
}

SteAlsAdaptorSysfs::~SteAlsAdaptorSysfs()
{
    delete alsBuffer;
}

void SteAlsAdaptorSysfs::processSample(int pathId, int fd)
{
    sensordLogC() << Q_FUNC_INFO;
    Q_UNUSED(pathId);

    char buf[16];
    lseek(fd, 0, SEEK_SET);
    if (read(fd, buf, sizeof(buf)) <= 0) {
        sensordLogW() << "read():" << strerror(errno);
        return;
    }

    sensordLogT() << "Ambient light value: " << buf;

    TimedUnsigned* lux = alsBuffer->nextSlot();
    sscanf(buf, "%d", &lux->value_);
    lux->timestamp_ = Utils::getTimeStamp();
    alsBuffer->commit();
    alsBuffer->wakeUpReaders();
}

bool SteAlsAdaptorSysfs::startSensor()
{
    sensordLogC() << Q_FUNC_INFO;
    if(!powerStatePath.isEmpty()) {
        writeToFile(powerStatePath, powerMode);
    }

    if (!(SysfsAdaptor::startSensor()))
        return false;

    return true;
}

void SteAlsAdaptorSysfs::stopSensor()
{
    sensordLogC() << Q_FUNC_INFO;
    if(!powerStatePath.isEmpty()) {
        writeToFile(powerStatePath, "0");
    }
    SysfsAdaptor::stopSensor();
}

