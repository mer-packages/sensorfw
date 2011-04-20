/**
   @file mrstaccelerometeradaptor.cpp
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "config.h"
#include "mrstaccelerometeradaptor.h"
#include "logging.h"
#include "datatypes/utils.h"

MRSTAccelAdaptor::MRSTAccelAdaptor (const QString& id) :
    SysfsAdaptor (id, SysfsAdaptor::IntervalMode)
{
    buffer = new DeviceAdaptorRingBuffer<OrientationData>(1);
    setAdaptedSensor("accelerometer", "MRST accelerometer", buffer);
    setDescription("MRST accelerometer");
}

MRSTAccelAdaptor::~MRSTAccelAdaptor () {
    delete buffer;
}

void MRSTAccelAdaptor::processSample (int, int fd) {
    char buf[32];
    int x, y, z;

    lseek (fd, 0, SEEK_SET);
    if ( read (fd, buf, sizeof(buf)) < 0 ) {
        sensordLogW () << "Read failed";
        return;
    }

    if ( sscanf (buf, "(%d,%d,%d)", &x, &y, &z) == 0 ) {
        sensordLogW () << "Wrong data format";
        return;
    }

    OrientationData* d = buffer->nextSlot ();
    d->timestamp_ = Utils::getTimeStamp();
    d->x_ = x;
    d->y_ = y;
    d->z_ = z;

    buffer->commit ();
    buffer->wakeUpReaders ();
}
