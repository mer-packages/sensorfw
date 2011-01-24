/**
   @file alsplugin.cpp
   @brief Plugin for ALSSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

#include "alsplugin.h"
#include "alssensor.h"
#include "sensormanager.h"
#include "logging.h"

void ALSPlugin::Register(class Loader&)
{
    sensordLogD() << "registering alssensor";
    SensorManager& sm = SensorManager::instance();
    sm.registerSensor<ALSSensorChannel>("alssensor");
}

void ALSPlugin::Init(class Loader& l)
{
    Q_UNUSED(l);
    SensorManager::instance().requestSensor("alssensor");
}

QStringList ALSPlugin::Dependencies() {
    return QString("alsadaptor").split(":", QString::SkipEmptyParts);
}

Q_EXPORT_PLUGIN2(alssensor, ALSPlugin)
