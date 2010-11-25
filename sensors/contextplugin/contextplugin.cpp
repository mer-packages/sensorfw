/**
   @file contextplugin.cpp
   @brief Plugin for ContextSensor
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>
   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

#include "contextplugin.h"
#include "sensormanager.h"
#include "contextsensor.h"
#include "logging.h"

void ContextPlugin::Register(class Loader&)
{
    sensordLogD() << "registering contextsensor";
    SensorManager& sm = SensorManager::instance();
    sm.registerSensor<ContextSensorChannel>("contextsensor");
}

void ContextPlugin::Init(class Loader&)
{
    SensorManager::instance().requestControlSensor("contextsensor");
}

QStringList ContextPlugin::Dependencies() {
    return QString("accelerometeradaptor:coordinatealignfilter:orientationchain").split(":", QString::SkipEmptyParts);
}

Q_EXPORT_PLUGIN2(contextsensor, ContextPlugin)
