/**
   @file mrstaccelerometeradaptor.h
   @brief Contains MRSTAccelerometerAdaptor.

   <p>
   Copyright (C) 2010 Nokia Corporation
   Copyright (C) 2010 Intel Corporation

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

#ifndef MRSTACCELEROMETERADPTOR_H
#define MRSTACCELEROMETERADPTOR_H

#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"

#include "datatypes/orientationdata.h"

class MRSTAccelAdaptor : public SysfsAdaptor
{
    Q_OBJECT

public:
    static DeviceAdaptor* factoryMethod (const QString& id)
    {
        return new MRSTAccelAdaptor (id);
    }

    MRSTAccelAdaptor (const QString& id);
    ~MRSTAccelAdaptor ();

protected:
    void processSample (int pathId, int fd);

private:
    DeviceAdaptorRingBuffer<OrientationData>* buffer;
};
#endif
