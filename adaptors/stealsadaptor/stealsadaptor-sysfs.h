/**
   @file alsadaptor-sysfs.h
   @brief ALSAdaptor that reads lux values from a sysfs node

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#ifndef STEALSADAPTOR_SYSFS_H
#define STEALSADAPTOR_SYSFS_H

#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/timedunsigned.h"


class SteAlsAdaptorSysfs : public SysfsAdaptor
{
    Q_OBJECT
public:
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new SteAlsAdaptorSysfs(id);
    }
    SteAlsAdaptorSysfs(const QString& id);
    ~SteAlsAdaptorSysfs();

    bool startSensor();
    void stopSensor();

protected:
    void processSample(int pathId, int fd);

private:
    DeviceAdaptorRingBuffer<TimedUnsigned>* alsBuffer;

    QByteArray powerStatePath;
    QByteArray powerMode;
};

#endif
