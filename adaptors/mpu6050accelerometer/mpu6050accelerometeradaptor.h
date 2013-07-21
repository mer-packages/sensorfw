/**
   Copyright 2013 Ruediger Gad <r.c.g@gmx.de>

   The Mpu6050AccelAdaptor was created based on the OemTabletAccelAdaptor.

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
*/

#ifndef MPU6050ACCELEROMETERADPTOR_H
#define MPU6050ACCELEROMETERADPTOR_H
#include "sysfsadaptor.h"
#include <QString>
#include <QStringList>
#include <linux/input.h>
#include "deviceadaptorringbuffer.h"
#include "datatypes/orientationdata.h"
#include <QTime>

#define X_AXIS 1
#define Y_AXIS 2
#define Z_AXIS 3

class Mpu6050AccelAdaptor : public SysfsAdaptor {
    Q_OBJECT

    public:
        static DeviceAdaptor* factoryMethod (const QString& id) {
            return new Mpu6050AccelAdaptor (id);
        }
        Mpu6050AccelAdaptor (const QString& id);
        ~Mpu6050AccelAdaptor ();

        bool startSensor ();
        void stopSensor ();

    protected:
        void processSample (int pathId, int fd);

    private:
        DeviceAdaptorRingBuffer<OrientationData>* buffer;
        OrientationData *currentData;
};
#endif
