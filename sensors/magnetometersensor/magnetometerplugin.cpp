/**
   @file magnetometerplugin.cpp
   @brief Plugin for MagnetometerSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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

#include "magnetometerplugin.h"
#include "magnetometersensor.h"
#include "sensormanager.h"
#include "sensord/logging.h"
#include <QtDebug>

void MagnetometerPlugin::Register(class Loader&)
{
    sensordLogD() << "registering magnetometersensor";
    SensorManager& sm = SensorManager::instance();
    sm.registerSensor<MagnetometerSensorChannel>("magnetometersensor");
}

QStringList MagnetometerPlugin::Dependencies() {
    return QString("compasschain").split(":", QString::SkipEmptyParts);
}

Q_EXPORT_PLUGIN2(magnetometersensor, MagnetometerPlugin)
