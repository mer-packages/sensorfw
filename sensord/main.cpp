/**
   @file main.cpp
   @brief Sensord initiation point

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Lihan Guo <ext-lihan.guo@nokia.com>

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
#include "calibrationhandler.h"
#include "datatypes/datarange.h"
#include "parser.h"

#ifdef USE_INTERNAL
#include "datatypes/tap.h"
#endif

#define CONFIG_FILE_PATH     "/etc/sensord.conf"

void printUsage();

int main(int argc, char *argv[])
{
    {sensordLog() << "Starting sensord...";}
    QCoreApplication app(argc, argv);   
    SensorManager& sm = SensorManager::instance();
    QStringList arguments = app.arguments();
    Parser *parser = new Parser(arguments);

    if (parser->printHelp())
    {
        printUsage();        
        app.exit(EXIT_SUCCESS);
        return 0;

    }


    if (parser->configFileInput())
    {  
        QString configFile = parser->configFilePath();
        QFile file(configFile);
        if (Config::loadConfig(configFile))  
            sensordLogT() << "Config file is loading successfully.";
        else
        {    
            sensordLogW() << "Config file error! Load default config file.";
            Config::loadConfig(CONFIG_FILE_PATH);
        }
    } else
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
    qDBusRegisterMetaType<DataRange>();

   // TODO: this results in error messages in case only Ariane is used

#ifdef USE_INTERNAL
    // Leave loading for client app. ALS loaded for context fw.
    //sensordLogD() << "Loading CompassSensor:" <<  sm.loadPlugin("compasssensor");
    //sensordLogD() << "Loading OrientationSensor:" << sm.loadPlugin("orientationsensor");
    sensordLogD() << "Loading ALSSensor:" << sm.loadPlugin("alssensor");
    //sensordLogD() << "Loading TapSensor:" << sm.loadPlugin("tapsensor");
    //sensordLogD() << "Loading AccelerometerSensor" << sm.loadPlugin("accelerometersensor");
    //sensordLogD() << "Loading ProximitySensor" << sm.loadPlugin("proximitysensor");
#endif
 
#ifdef PROVIDE_CONTEXT_INFO    
 
    if (parser->contextInfo())
    {
        sensordLogD() << "Loading ContextSensor" << sm.loadPlugin("contextsensor");
        // FIXME: A HACK: make sure the AlsSensorChannel & ContextSensorChannel are created
        sm.requestControlSensor("alssensor");
        sm.requestControlSensor("contextsensor");
    }
    
#endif


    if (parser->createDaemon())
    {
        int pid; 
        pid = fork();
        
        if(pid < 0)
        {
            sensordLogC() << "Failed to create a daemon.";
            exit(EXIT_FAILURE);
        } else {
            if (pid > 0)
            {    
                sensordLogW() << "Create a daemon";
                exit(EXIT_SUCCESS);
            }
        }
    }


    if (parser->magnetometerCalibration())
    {
        CalibrationHandler* calibrationHandler_ = new CalibrationHandler(NULL);
        calibrationHandler_->initiateSession();
        QObject::connect(&sm, SIGNAL(displayOn()), calibrationHandler_, SLOT(resumeCalibration()));
    }


    if (parser->changeLogLevel())
    {
        SensordLogLevel level = parser->getLogLevel();    
        SensordLogger* logger = new SensordLogger("Sensord", __PRETTY_FUNCTION__, __FILE__, __LINE__, level);
        logger -> setOutputLevel(level);
        delete logger;
    }


    if (!sm.registerService())
    {
        sensordLogW() << "Failed to register service on D-Bus. Aborting.";
        exit(EXIT_FAILURE);
    }

    delete parser;
    return app.exec();
}


void printUsage()
{
    qDebug() << "Usage: sensord [OPTIONS]";
    qDebug() << " -d, --daemon                     Detach from terminal and run as daemon.\n";
    qDebug() << " -l=N, --log-level=N              Use logging level N. Messages are logged for";    
    qDebug() << "                                  the given and higher priority levels. Level";    
    qDebug() << "                                  can also be notched up by sending SIGUSR1 to";    
    qDebug() << "                                  the process. Valid values for N are: 'test',";    
    qDebug() << "                                  'debug', 'warning', 'critical'.\n";    
    qDebug() << " -c=P, --config-file=P            Load configuration from P. By default";
    qDebug() << "                                  /etc/sensord.conf is used.\n";
    qDebug() << " --no-context-info                Do not provide context information for context";
    qDebug() << "                                  /etc/sensord.conf is used.\n";
    qDebug() << " --no-magnetometer-bg-calibration Do not start calibration of magnetometer in";
    qDebug() << "                                  the background.\n";
    qDebug() << " -h, --help                       Show usage info and exit."; 
}
