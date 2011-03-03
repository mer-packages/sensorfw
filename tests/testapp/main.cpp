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
#include <signal.h>

#include "sensorhandler.h"
#include "parser.h"
#include "config.h"
#include "logging.h"
#include "statprinter.h"

void printUsage();

void sigIntHandler(int param)
{
    Q_UNUSED(param);
    QCoreApplication::exit(0);
}

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

    signal(SIGINT, sigIntHandler);

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

    SensorHandler::init(Config::configuration()->groups());

    QList<SensorHandler*> handlers;
    foreach (const QString& sensorName, Config::configuration()->groups())
    {
        int count = Config::configuration()->value(sensorName + "/instances", "0").toInt();
        for(int i = 0; i < count; ++i)
        {
            SensorHandler* handler = new SensorHandler(sensorName, &app);
            if (parser.singleThread()) {
                handler->startClient();
            } else {
                handler->start();
            }
            handlers << handler;
        }
    }
    StatPrinter* printer = 0;
    if(parser.statInterval())
    {
        printer = new StatPrinter(handlers, parser.statInterval(), &app);
    }
    sensordLogD() << "Threads are waiting.";
    int ret = app.exec();
    if(!parser.gracefulShutdown())
        return ret;
    sensordLogD() << "Exiting...";
    delete printer;
    foreach(SensorHandler* handler, handlers)
    {
        handler->stopClient();
        if (!parser.singleThread() && handler->isRunning())
            handler->quit();
        delete handler;
    }
    Config::close();
    SensordLogger::close();
    return ret;
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
    qDebug() << " -m=N  --model=N                  Start clients in single thread model or multithread mode. (1(default)=single thread, 2=multithread)\n";
    qDebug() << " -i=N, --stat-interval=N          Interval for statistics printing.\n";
    qDebug() << " -g=N                             Perform graceful shutdown (1(default)=yes, 0=no).\n";
    qDebug() << " -h, --help                       Show usage info and exit.";
}
