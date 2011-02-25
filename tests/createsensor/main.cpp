/**
   @file main.cpp
   @brief test application to create sensor
   <p>
   Copyright (C) 2010-2011 Nokia Corporation

   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>

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

#include <QCoreApplication>
#include <QTest>
#include <QVector>
#include <QThread>
#include <QMap>
#include <math.h>
#include <QMapIterator>
#include <QDebug>

#include "qt-api/sensormanagerinterface.h"
#include "qt-api/orientationsensor_i.h"
#include "qt-api/accelerometersensor_i.h"
#include "qt-api/compasssensor_i.h"
#include "qt-api/tapsensor_i.h"
#include "qt-api/alssensor_i.h"
#include "qt-api/proximitysensor_i.h"
#include "qt-api/rotationsensor_i.h"
#include "qt-api/magnetometersensor_i.h"
#include "datatypes/magneticfield.h"

#include "testthread.h"
#include "parser.h"
#include "config.h"

int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);
    QMap<QString, int> sensorThread;

    const char* CONFIG_FILE_PATH = "/usr/share/sensord-tests/createsensors.conf";

    Parser parser(app.arguments());
    sensorThread = parser.getSensorThread();
    if (sensorThread.empty()){
        return 0;
    }

    Config::loadConfig(CONFIG_FILE_PATH, "");

    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();

    foreach (const QString& sensorName_, sensorThread.keys())
    {
        remoteSensorManager.loadPlugin(sensorName_);
        
        if (sensorName_ == "orientationsensor"){
            remoteSensorManager.registerSensorInterface<OrientationSensorChannelInterface>("orientationsensor");
        } else if (sensorName_ == "accelerometersensor"){
            remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");
        } else if (sensorName_ == "compasssensor"){
            remoteSensorManager.registerSensorInterface<CompassSensorChannelInterface>("compasssensor");
        } else if (sensorName_ == "tapsensor"){
            remoteSensorManager.registerSensorInterface<TapSensorChannelInterface>("tapsensor");
        } else if (sensorName_ == "alssensor"){
            remoteSensorManager.registerSensorInterface<ALSSensorChannelInterface>("alssensor");
        } else if (sensorName_ == "proximitysensor"){
            remoteSensorManager.registerSensorInterface<ProximitySensorChannelInterface>("proximitysensor");
        } else if (sensorName_ == "rotationsensor"){
            remoteSensorManager.registerSensorInterface<RotationSensorChannelInterface>("rotationsensor");
        } else if (sensorName_ == "magnetometersensor"){
            remoteSensorManager.registerSensorInterface<MagnetometerSensorChannelInterface>("magnetometersensor");
        }
    }

  
    QMapIterator<QString, int> iterator(sensorThread);

    while (iterator.hasNext()) {

        iterator.next();
        if (iterator.key() == "orientationsensor"){
            for (int i = 0; i < iterator.value(); i++){
                (new Testthread("orientationsensor")) -> start();
            }
        } 

        if (iterator.key() == "accelerometersensor"){
            for (int i = 0; i < iterator.value(); i++){
                (new Testthread("accelerometersensor")) -> start();
            }
        } 


        if (iterator.key() == "compasssensor"){
            for (int i = 0; i < iterator.value(); i++){
                (new Testthread("compasssensor")) -> start();
            }
        }
        if (iterator.key() == "tapsensor"){
            for (int i = 0; i < iterator.value(); i++){
                (new Testthread("tapsensor")) -> start();
            }
        } 

        if (iterator.key() == "alssensor"){
            for (int i = 0; i < iterator.value(); i++){
                (new Testthread("alssensor")) -> start();
            }
        } 

        if (iterator.key() == "proximitysensor"){
            for (int i = 0; i < iterator.value(); i++){
                (new Testthread("proximitysensor")) -> start();
            }
        } 

        if (iterator.key() == "rotationsensor"){
            for (int i = 0; i < iterator.value(); i++){
                (new Testthread("rotationsensor")) -> start();
            }
        } 

        if (iterator.key() == "magnetometersensor"){
            for (int i = 0; i < iterator.value(); i++){
                (new Testthread("magnetometersensor")) -> start();
            }
        } 
    }

    qDebug() << "Threads are waiting.";        
    return app.exec();
}



