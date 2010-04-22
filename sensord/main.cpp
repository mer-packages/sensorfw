/**
   @file main.cpp
   @brief Sensord initiation point

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
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

#include <QtCore/QCoreApplication>
#include <QDBusConnection>

#include "config.h"
#include "sensormanager.h"
#include "sensormanager_a.h"
#include "datatypes/xyz.h"
#include "datatypes/magneticfield.h"
#include "datatypes/unsigned.h"
#include "datatypes/compass.h"
#include "logging.h"
#include "datatypes/orientation.h"

#ifdef USE_INTERNAL
#include "datatypes/tap.h"
#endif

#define CONFIG_FILE_PATH     "/etc/sensord.conf"

int main(int argc, char *argv[])
{
    {sensordLog() << "Starting sensord...";}
    QCoreApplication app(argc, argv);

    Config::loadConfig(CONFIG_FILE_PATH);

    // TODO: move these to plugins...
    qDBusRegisterMetaType<XYZ>();
    qDBusRegisterMetaType<Compass>();
    qDBusRegisterMetaType<Unsigned>();
    qDBusRegisterMetaType<Orientation>();
    qDBusRegisterMetaType<MagneticField>();
#ifdef USE_INTERNAL
    qDBusRegisterMetaType<Tap>();
#endif


    SensorManager& sm = SensorManager::instance();

// TODO: this results in error messages in case only Ariane is used
#ifdef USE_INTERNAL
    // Leave loading for client app. ALS loaded for context fw.
    //sensordLog() << "Loading CompassSensor:" <<  sm.loadPlugin("compasssensor");
    //sensordLog() << "Loading OrientationSensor:" << sm.loadPlugin("orientationsensor");
    sensordLog() << "Loading ALSSensor:" << sm.loadPlugin("alssensor");
    //sensordLog() << "Loading TapSensor:" << sm.loadPlugin("tapsensor");
    //sensordLog() << "Loading AccelerometerSensor" << sm.loadPlugin("accelerometersensor");
    //sensordLog() << "Loading ProximitySensor" << sm.loadPlugin("proximitysensor");
#endif

#ifdef PROVIDE_CONTEXT_INFO    
    sensordLog() << "Loading ContextSensor" << sm.loadPlugin("contextsensor");

    // FIXME: A HACK: make sure the AlsSensorChannel & ContextSensorChannel are created
    sm.requestControlSensor("alssensor");
    sm.requestControlSensor("contextsensor");

#endif

    sm.registerService();
    
    return app.exec();
}
