/**
   @file mrstaccelerometeradaptorplugin.cpp
   @brief Contains MRSTAccelerometerAdaptorPlugin.

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

#include "mrstaccelerometeradaptorplugin.h"
#include "mrstaccelerometeradaptor.h"
#include "sensormanager.h"
#include "logging.h"

void MRSTAccelerometerAdaptorPlugin::Register(class Loader&)
{
    sensordLogD() << "registering mrstaccelerometeradaptor";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<MRSTAccelAdaptor>("accelerometeradaptor");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(accelerometeradaptor-mrst, MRSTAccelerometerAdaptorPlugin)
#endif
