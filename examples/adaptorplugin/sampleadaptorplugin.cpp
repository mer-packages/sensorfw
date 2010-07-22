/**
   @file sampleadaptorplugin.cpp
   @brief Sample adaptor plugin

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

#include "sampleadaptorplugin.h"
#include "sampleadaptor.h"
#include "sensormanager.h"

void SampleAdaptorPlugin::Register(class Loader&)
{
    SensorManager& sm = SensorManager::instance();

    // All adaptors provided by the plugin should be registered to the
    // sensormanager with a recognisable name. So far the practice has
    // been to use the type of the node in the name as well, thus
    // "xxxadaptor"
    sm.registerDeviceAdaptor<SampleAdaptor>("sampleadaptor");
}

// Plugin must be exported for it to loaded properly
Q_EXPORT_PLUGIN2(sampleadaptor, SampleAdaptorPlugin)
