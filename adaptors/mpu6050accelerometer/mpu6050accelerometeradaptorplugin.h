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

#ifndef MPU6050ACCELEROMETERADAPTORPLUGIN_H
#define MPU6050ACCELEROMETERADAPTORPLUGIN_H

#include "plugin.h"

class Mpu6050AccelerometerAdaptorPlugin : public Plugin
{
    Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "com.nokia.SensorService.Plugin/1.0")
#endif

private:
    void Register(class Loader& l);
};

#endif
