/**
   @file sampleplugin.cpp
   @brief Sample plugin for sensors

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

#include "sampleplugin.h"
#include "samplesensor.h"
#include "sensormanager.h"
#include <QtDebug>

void SamplePlugin::Register(class Loader&)
{
    qDebug() << "registering samplesensor";
    SensorManager& sm = SensorManager::instance();
    sm.registerSensor<SampleSensorChannel>("samplesensor");
}

// We only directly interface with the sample chain, thus it is our only
// dependency here. We can rely on it to provide further dependencies to
// any plugins it may need.
QStringList SamplePlugin::Dependencies() {
    return QString("samplechain").split(":", QString::SkipEmptyParts);
}

Q_EXPORT_PLUGIN2(samplesensor, SamplePlugin)
