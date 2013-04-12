/**
   @file downsamplefilterplugin.cpp
   @brief Plugin for DownsampleFilter

   <p>
   Copyright (C) 2011 Nokia Corporation

   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#include "downsamplefilterplugin.h"
#include "downsamplefilter.h"
#include "sensormanager.h"
#include "logging.h"

void DownsampleFilterPlugin::Register(class Loader&)
{
    sensordLogD() << "registering downsamplefilter";
    SensorManager& sm = SensorManager::instance();
    sm.registerFilter<DownsampleFilter>("downsamplefilter");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(downsamplefilter, DownsampleFilterPlugin)
#endif
