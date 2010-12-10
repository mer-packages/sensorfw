/**
   @file gyroscopeplugin.cpp
   @brief Plugin for GyroscopeSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>

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

#include "gyroscopeplugin.h"
#include "gyroscopesensor.h"
#include "sensormanager.h"
#include <QtDebug>

void GyroscopePlugin::Register(class Loader&)
{
    sensordLogD() << "registering gyroscopesensor";
    SensorManager& sm = SensorManager::instance();
    sm.registerSensor<GyroscopeSensorChannel>("gyroscopesensor");
}

QStringList GyroscopePlugin::Dependencies() {
    return QString("gyroscopeadaptor").split(":", QString::SkipEmptyParts);
}

Q_EXPORT_PLUGIN2(gyroscopesensor, GyroscopePlugin)
