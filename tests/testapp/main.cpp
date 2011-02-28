/**
   @file main.cpp
   @brief test application to create sensor
   <p>
   Copyright (C) 2010-2011 Nokia Corporation

   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
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

#include <QCoreApplication>
#include <QTest>
#include <QList>
#include <QString>
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

#include "sensorhandler.h"
#include "parser.h"
#include "config.h"
#include "logging.h"

void printUsage();

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    const char* CONFIG_FILE_PATH = "/usr/share/sensord-tests/testapp.conf";

    Parser parser(app.arguments());

    if (parser.printHelp())
    {
        printUsage();
        app.exit(EXIT_SUCCESS);
        return 0;
    }

    SensordLogger::init(parser.logTarget(), parser.logFilePath());

    if (parser.changeLogLevel())
    {
        SensordLogger::setOutputLevel(parser.getLogLevel());
    }

    if (parser.configFileInput())
    {
        QString defConfigFile = parser.configFilePath();
        if (Config::loadConfig(defConfigFile, ""))
            sensordLogT() << "Config file is loading successfully.";
        else
        {
            sensordLogW() << "Config file error! Load using default path.";
            Config::loadConfig(CONFIG_FILE_PATH, "");
        }
    } else {
        Config::loadConfig(CONFIG_FILE_PATH, "");
    }

    if (Config::configuration() == NULL)
    {
        sensordLogC() << "Failed to load configuration. Aborting.";
        exit(EXIT_FAILURE);
    }

    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();

    foreach (const QString& sensorName, Config::configuration()->groups())
    {
        remoteSensorManager.loadPlugin(sensorName);

        if (sensorName == "orientationsensor"){
            remoteSensorManager.registerSensorInterface<OrientationSensorChannelInterface>(sensorName);
        } else if (sensorName == "accelerometersensor"){
            remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>(sensorName);
        } else if (sensorName == "compasssensor"){
            remoteSensorManager.registerSensorInterface<CompassSensorChannelInterface>(sensorName);
        } else if (sensorName == "tapsensor"){
            remoteSensorManager.registerSensorInterface<TapSensorChannelInterface>(sensorName);
        } else if (sensorName == "alssensor"){
            remoteSensorManager.registerSensorInterface<ALSSensorChannelInterface>(sensorName);
        } else if (sensorName == "proximitysensor"){
            remoteSensorManager.registerSensorInterface<ProximitySensorChannelInterface>(sensorName);
        } else if (sensorName == "rotationsensor"){
            remoteSensorManager.registerSensorInterface<RotationSensorChannelInterface>(sensorName);
        } else if (sensorName == "magnetometersensor"){
            remoteSensorManager.registerSensorInterface<MagnetometerSensorChannelInterface>(sensorName);
        }
    }

    QList<SensorHandler*> handlers;
    foreach (const QString& sensorName, Config::configuration()->groups())
    {
        int count = Config::configuration()->value(sensorName + "/instances", "0").toInt();
        for(int i = 0; i < count; ++i)
        {
            SensorHandler* handler = new SensorHandler(sensorName);
            handler->startClient();
            handlers << handler;
        }
    }
    sensordLogD() << "Threads are waiting.";
    return app.exec();
}

void printUsage()
{
    qDebug() << "Usage: sensortestapp [OPTIONS]";
    qDebug() << " -l=N, --log-level=N              Use logging level N. Messages are logged for";
    qDebug() << "                                  the given and higher priority levels. Level";
    qDebug() << "                                  can also be notched up by sending SIGUSR1 to";
    qDebug() << "                                  the process. Valid values for N are: 'test',";
    qDebug() << "                                  'debug', 'warning', 'critical'.\n";
    qDebug() << " --log-target=N                   logging target mask (1=stdout, 2=stderr, 4=file, 8=syslog and combos e.g. 3=stdout|stderr\n";
    qDebug() << " --log-file-path=P                Log file path\n";
    qDebug() << " -c=P, --config-file=P            Load configuration from P. By default";
    qDebug() << "                                  /usr/share/sensord-tests/testapp.conf is used.\n";
    qDebug() << " -h, --help                       Show usage info and exit.";
}
