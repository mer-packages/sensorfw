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
#include <QList>
#include <QString>
#include <QDebug>
#include <signal.h>

#include "parser.h"
#include "config.h"
#include "logging.h"
#include "clientadmin.h"

void printUsage();

void sigIntHandler(int param)
{
    Q_UNUSED(param);
    QCoreApplication::exit(0);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Parser parser(app.arguments());

    if (parser.printHelp())
    {
        printUsage();
        app.exit(EXIT_SUCCESS);
        return 0;
    }

    signal(SIGINT, sigIntHandler);

    ClientAdmin clientAdmin(parser);
    clientAdmin.runClients();

    int ret = app.exec();

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
    qDebug() << "                                  /usr/share/sensorfw-tests/testapp.conf is used.\n";
    qDebug() << " -m=N  --model=N                  Start clients in single thread model or multithread mode. (1=single thread(default), 2=multithread)\n";
    qDebug() << " -i=N, --stat-interval=N          Interval for statistics printing.\n";
    qDebug() << " -g=N                             Perform graceful shutdown (1=yes(default), 0=no).\n";
    qDebug() << " -h, --help                       Show usage info and exit.";
}
