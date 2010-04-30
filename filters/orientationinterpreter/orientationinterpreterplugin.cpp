/**
   @file orientationinterpreterplugin.cpp
   @brief Plugin for OrientationInterpreterFilter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#include "orientationinterpreterplugin.h"
#include "orientationinterpreter.h"
#include "sensormanager.h"
#include "sensord/logging.h"

void OrientationInterpreterPlugin::Register(class Loader&)
{
    sensordLogD() << "registering orientationinterpreter";
    SensorManager& sm = SensorManager::instance();
    sm.registerFilter<OrientationInterpreter>("orientationinterpreter");
}

Q_EXPORT_PLUGIN2(orientationinterpreter, OrientationInterpreterPlugin)
