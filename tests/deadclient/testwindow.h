/**
   @file testwindow.h
   @brief Window for dead client tests
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#include <QObject>
#include <orientationsensor_i.h>
#include <accelerometersensor_i.h>
#include <compasssensor_i.h>
#include <rotationsensor_i.h>
#include <magnetometersensor_i.h>
#include <sensormanagerinterface.h>
#include <datatypes/xyz.h>
#include <datatypes/unsigned.h>
#include "datareceiver.h"

class TestWindow
{
public:

    TestWindow(bool control = false)
    {
        SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();

        remoteSensorManager.loadPlugin("orientationsensor");
        remoteSensorManager.loadPlugin("accelerometersensor");
        remoteSensorManager.loadPlugin("compasssensor");
        remoteSensorManager.loadPlugin("rotationsensor");
        remoteSensorManager.loadPlugin("magnetometersensor");

        remoteSensorManager.registerSensorInterface<OrientationSensorChannelInterface>("orientationsensor");
        remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");
        remoteSensorManager.registerSensorInterface<CompassSensorChannelInterface>("compasssensor");  
        remoteSensorManager.registerSensorInterface<RotationSensorChannelInterface>("rotationsensor");
        remoteSensorManager.registerSensorInterface<MagnetometerSensorChannelInterface>("magnetometersensor");

#ifdef ORIENTATION
        if (control) {
            sensorIfc = const_cast<OrientationSensorChannelInterface*>(OrientationSensorChannelInterface::interface("orientationsensor"));
            QObject::connect(sensorIfc, SIGNAL(orientationChanged(const Unsigned&)), &dump, SLOT(dump(const Unsigned&)));
        }
#else
        if (control) {
            sensorIfc = const_cast<RotationSensorChannelInterface*>(RotationSensorChannelInterface::interface("rotationsensor"));
            QObject::connect(sensorIfc, SIGNAL(dataAvailable(const XYZ&)), &dump, SLOT(dump(const XYZ&)));
        }
#endif
        sensorIfc->start();
    }

public slots:
    void cleanup() {
        sensorIfc->stop();
        delete sensorIfc;
    }
public:
    
    DataReceiver dump;
#ifdef ORIENTATION
    OrientationSensorChannelInterface* sensorIfc;
#else
    RotationSensorChannelInterface* sensorIfc;
#endif    
};
