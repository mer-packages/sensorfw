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

#include "mpu6050accelerometeradaptorplugin.h"
#include "mpu6050accelerometeradaptor.h"
#include "sensormanager.h"
#include "logging.h"

void Mpu6050AccelerometerAdaptorPlugin::Register(class Loader&)
{
    sensordLogD() << "registering mpu6050accelerometeradaptor";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<Mpu6050AccelAdaptor>("accelerometeradaptor");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(mpu6050accelerometeradaptor, Mpu6050AccelerometerAdaptorPlugin)
#endif
