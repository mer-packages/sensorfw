/**
   @file clientadmin.cpp
   @brief base class for sensorhandlers

   <p>
   Copyright (C) 2011 Nokia Corporation

   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>

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

#include "clientadmin.h"

const char* ClientAdmin::CONFIG_FILE_PATH = "/usr/share/sensord-tests/testapp.conf";

ClientAdmin::ClientAdmin(const Parser& parser, QObject *parent) :
        QObject(parent),parser(parser), printer(0)
{
}

void ClientAdmin::init()
{
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

    registerSensorInterface(Config::configuration()->groups());
}

void ClientAdmin::runClients()
{
    init();

    foreach (const QString& sensorName, Config::configuration()->groups())
    {
        int count = Config::configuration()->value(sensorName + "/instances", "0").toInt();
        for(int i = 0; i < count; ++i)
        {
            SensorHandler* handler = new SensorHandler(sensorName, this);
            if (parser.singleThread()) {
                handler->startClient();
            } else {
                handler->start();
            }
            handlers << handler;
        }
    }

    if(parser.statInterval())
    {
        printer = new StatPrinter(handlers, parser.statInterval(), this);
    }

    sensordLogD() << "Threads are waiting.";
}

bool ClientAdmin::registerSensorInterface(const QStringList& sensors)
{
    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
    if(!remoteSensorManager.isValid())
    {
        sensordLogC() << "Failed to create SensorManagerInterface";
        return false;
    }
    foreach (const QString& sensorName, sensors)
    {
        QDBusReply<bool> reply(remoteSensorManager.loadPlugin(sensorName));
        if(!reply.isValid() || !reply.value())
        {
            sensordLogW() << "Failed to load plugin";
            return false;
        }

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
}

ClientAdmin::~ClientAdmin()
{
    if(!parser.gracefulShutdown())
        return;
    sensordLogD() << "Exiting...";

    delete printer;
    foreach(SensorHandler* handler, handlers)
    {
        if (parser.singleThread())
        {
            handler->stopClient();
        }else {
            handler->exit(0);
            handler->wait();
        }
        delete handler;
    }
    Config::close();
    SensordLogger::close();
}
