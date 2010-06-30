/**
   @file alsadaptor.cpp
   @brief ALSAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#include "logging.h"
#include "config.h"
#include "alsadaptor.h"
#include <errno.h>
#include "filters/utils.h"
#include <linux/types.h>

struct bh1770glc_als {
    __u16 lux;
} __attribute__((packed));

ALSAdaptor::ALSAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::SelectMode, Config::configuration()->value("als_dev_path").toString())
{
    alsBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(1024);
    addAdaptedSensor("als", "Internal ambient light sensor lux values", alsBuffer_);
}

ALSAdaptor::~ALSAdaptor()
{
    delete alsBuffer_;
}

void ALSAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    struct bh1770glc_als als_data;
    als_data.lux = 0;

    int bytesRead = read(fd, &als_data, sizeof(als_data));

    if (bytesRead <= 0) {
        sensordLogW() << "read():" << strerror(errno);
        return;
    }
    sensordLogT() << "Ambient light value: " << als_data.lux;

    TimedUnsigned* lux = alsBuffer_->nextSlot();
    lux->value_ = als_data.lux;

    lux->timestamp_ = Utils::getTimeStamp();

    alsBuffer_->commit();
    alsBuffer_->wakeUpReaders();
}
