/**
   @file magnetometeradaptor-ascii.h
   @brief MagnetometerAdaptor that reads magnetic value in X-axis, Y-axis, and Z-axis from ascii interface

   <p>
   Copyright (C) 2009-2010 Intel Corporation
   Copyright (C) 2009-2010 Nokia Corporation

   @author Leo Yan <leo.yan@intel.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
   @author Tapio Rantala <ext-tapio.rantala@nokia.com>
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

#ifndef MAGNETOMETERADAPTOR_ASCII_H
#define MAGNETOMETERADAPTOR_ASCII_H

#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/timedunsigned.h"

class MagnetometerAdaptorAscii : public SysfsAdaptor
{
    Q_OBJECT;
public:
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new MagnetometerAdaptorAscii(id);
    }

protected:
    MagnetometerAdaptorAscii(const QString& id);
    ~MagnetometerAdaptorAscii();

private:
    void processSample(int pathId, int fd);
    char buf[32];

    DeviceAdaptorRingBuffer<TimedXyzData>* magnetBuffer_;
};

#endif
