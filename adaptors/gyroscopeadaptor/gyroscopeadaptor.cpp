/**
   @file gyroscopeadaptor.cpp
   @brief GyroscopeAdaptor based on SysfsAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>
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

#include "logging.h"
#include "config.h"
#include "datatypes/utils.h"

#include "gyroscopeadaptor.h"

GyroscopeAdaptor::GyroscopeAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::SelectMode)
{
    gyroscopeBuffer_ = new DeviceAdaptorRingBuffer<AngularVelocityData>(32);
    setAdaptedSensor("gyroscope", "l3g4200dh", gyroscopeBuffer_);
}

GyroscopeAdaptor::~GyroscopeAdaptor()
{
    delete gyroscopeBuffer_;
}

void GyroscopeAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);
    short x, y, z;
    char buf[32];

    if (read(fd, buf, sizeof(buf)) <= 0) {
        sensordLogW() << "read():" << strerror(errno);
        return;
    }
    sensordLogT() << "gyroscope output value: " << buf;

    sscanf(buf, "%hd %hd %hd\n", &x, &y, &z);

    AngularVelocityData* pos = gyroscopeBuffer_->nextSlot();
    pos->x_ = x;
    pos->y_ = y;
    pos->z_ = z;
    pos->timestamp_ = Utils::getTimeStamp();

    gyroscopeBuffer_->commit();
    gyroscopeBuffer_->wakeUpReaders();
}
