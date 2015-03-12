/**
   @file alsadaptorplugin-sysfs.cpp
   @brief Plugin for ALSAdaptorSysfs

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2015 Jolla

   @author Lorn Potter <lorn.potter@jolla.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Markus Lehtonen <markus.lehtonen@nokia.com>

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

#include "alsadaptor-evdevplugin.h"
#include "alsevdevadaptor.h"
#include "sensormanager.h"
#include "logging.h"

void ALSAdaptorEvdevPlugin::Register(class Loader&)
{
    sensordLogD() << "registering alsadaptor-evdev";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<ALSAdaptorEvdev>("alsadaptor");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(alsadaptor-evdev, ALSAdaptorEvdevPlugin)
#endif
