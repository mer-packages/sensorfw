/**
   @file main.cpp
   @brief Sensord initiation point

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com>
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Lihan Guo <ext-lihan.guo@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com

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
#include <signal.h>
#include <iostream>
#include <errno.h>

#include "config.h"
#include "sensormanager.h"
#include "sensormanager_a.h"
#include "logging.h"
#include "calibrationhandler.h"
#include "parser.h"

void printUsage();

void signalHandler(int param)
{
    Q_UNUSED(param);

    SensordLogger::setOutputLevel(static_cast<SensordLogLevel> ((static_cast<int>(SensordLogger::getOutputLevel()) + 1) % SensordLogN));

    std::cerr << "New debugging level: " << SensordLogger::getOutputLevel() << "\n";
}

void signalFlush(int param)
{
    Q_UNUSED(param);

    QStringList output;

    output.append("Flushing sensord state\n");
    output.append(QString("  Logging level: %1\n").arg(SensordLogger::getOutputLevel()));
    SensorManager::instance().printStatus(output);

    foreach (QString line, output) {
        sensordLogW() << line.toLocal8Bit().data();
    }
}

int main(int argc, char *argv[])
{
    const char* CONFIG_FILE_PATH = "/etc/sensorfw/sensord.conf";
    const char* CONFIG_DIR_PATH = "/etc/sensorfw/sensord.conf.d/";

    QCoreApplication app(argc, argv);
    SensorManager& sm = SensorManager::instance();
    Parser parser(app.arguments());
    SensordLogger::init(parser.logTarget(), parser.logFilePath());

    if (parser.printHelp())
    {
        printUsage();
        app.exit(EXIT_SUCCESS);
        return 0;

    }

    if (parser.changeLogLevel())
    {
        SensordLogger::setOutputLevel(parser.getLogLevel());
    }

    if (parser.configFileInput())
    {
        QString defConfigFile = parser.configFilePath();
        QFile file(defConfigFile);
        if (Config::loadConfig(defConfigFile, CONFIG_DIR_PATH))
            sensordLogT() << "Config file is loading successfully.";
        else
        {
            sensordLogW() << "Config file error! Load using default path.";
            Config::loadConfig(CONFIG_FILE_PATH, CONFIG_DIR_PATH);
        }
    } else {
        Config::loadConfig(CONFIG_FILE_PATH, CONFIG_DIR_PATH);
    }

    if (Config::configuration() == NULL)
    {
        sensordLogC() << "Failed to load configuration. Aborting.";
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalFlush);

#ifdef PROVIDE_CONTEXT_INFO
    if (parser.contextInfo())
    {
        sensordLogD() << "Loading ContextSensor " << sm.loadPlugin("contextsensor");
        sensordLogD() << "Loading ALSSensor " << sm.loadPlugin("alssensor");
    }
#endif

    if (parser.createDaemon())
    {
        int pid = fork();

        if(pid < 0)
        {
            sensordLogC() << "Failed to create a daemon: " << strerror(errno);
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            sensordLogW() << "Created a daemon";
            exit(EXIT_SUCCESS);
        }
    }

    if (parser.magnetometerCalibration())
    {
        CalibrationHandler* calibrationHandler_ = new CalibrationHandler(NULL);
        calibrationHandler_->initiateSession();
        QObject::connect(&sm, SIGNAL(resumeCalibration()), calibrationHandler_, SLOT(resumeCalibration()));
        QObject::connect(&sm, SIGNAL(stopCalibration()), calibrationHandler_, SLOT(stopCalibration()));
    }

    if (!sm.registerService())
    {
        sensordLogW() << "Failed to register service on D-Bus. Aborting.";
        exit(EXIT_FAILURE);
    }

    int ret = app.exec();
    Config::close();
    SensordLogger::close();
    return ret;
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
    qDebug() << " --log-target=N                   logging target mask (1=stdout, 2=stderr, 4=file, 8=syslog and combos e.g. 3=stdout|stderr\n";
    qDebug() << " --log-file-path=P                Log file path\n";
    qDebug() << " -c=P, --config-file=P            Load configuration from P. By default";
    qDebug() << "                                  /etc/sensord.conf is used.\n";
    qDebug() << " --no-context-info                Do not provide context information for context";
    qDebug() << "                                  framework.\n";
    qDebug() << " --no-magnetometer-bg-calibration Do not start calibration of magnetometer in";
    qDebug() << "                                  the background.\n";
    qDebug() << " -h, --help                       Show usage info and exit.";
}
