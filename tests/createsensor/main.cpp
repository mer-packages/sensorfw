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

int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);
    QStringList sensors;

    sensors << "orientationsensor" << "accelerometersensor" 
            << "compasssensor" << "tapsensor" << "alssensor"
            << "proximitysensor" << "rotationsensor" << "magnetometersensor" ;

    QMap<QString, int> sensorThread;

    QString sensorName = NULL;
    int numberOfThread;
    int i = 0;

    QStringList args = app.arguments();
    if (args.size() == 1){
        qDebug() << "Usage: sensorthread-test sensor1 num1 sensor2 num2 ....";
        qDebug() <<  "sensors:";
        qDebug() << "orientationsensor, " << "accelerometersensor"; 
        qDebug()  << "compasssensor, " << "tapsensor, " << "alssensor";
        qDebug()  << "proximitysensor, " << "rotationsensor ," << "magnetometersensor" ;
        app.exit();
        return 0;
    }

    args.removeFirst();
    foreach (const QString& arg, args)
    { 
        switch(i)
        {    
        case 0:
            sensorName = arg.trimmed();
            if (sensors.contains(sensorName)){
                i++;
            } else {
                qDebug() << sensorName << " doesn't exist, skip it";    
            }
            break;
        case 1:
            numberOfThread = arg.trimmed().toInt();
            qDebug() << sensorName << ": " << numberOfThread << " threads to run";
            sensorThread.insert(sensorName, numberOfThread);
            i = 0;
            break;
         }
    }

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



