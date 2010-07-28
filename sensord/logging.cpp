/**
   @file logging.cpp
   @brief Syslog utility

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

#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <QDateTime>
#include <QFile>
#include <iostream>
#include <signal.h>
#include <sensormanager.h>

#ifdef SENSORD_USE_SYSLOG
#include <syslog.h>
#endif

SensordLogLevel SensordLogger::outputLevel = SensordLogWarning;

SensordLogger::SensordLogger (const char *module, const char* func, 
                              const char *file, int line, SensordLogLevel level)
    : QTextStream(), moduleName(module), currentLevel(level)
{
#ifdef SENSORD_USE_SYSLOG
    openlog(NULL, LOG_PERROR, LOG_DAEMON);
#endif
    printLog = ((level >= SensordLogTest) && (level < SensordLogN)) && (level >= outputLevel);

    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalFlush);

    setString(&data);

    //add level
    switch (level) {
        case SensordLogTest:
            *this << QString("*TEST*").toLocal8Bit().data();
            break;
        case SensordLogDebug:
            *this << QString("*DEBUG*").toLocal8Bit().data();
            break;
        case SensordLogWarning:
            *this << QString("*WARNING*").toLocal8Bit().data();
            break;
        case SensordLogCritical:
            *this << QString("*CRITICAL*").toLocal8Bit().data();
            break;
        case SensordLogN:
        default:
            break;
    }

    //add time stamp
    *this << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data();

#ifndef SENSORD_USE_SYSLOG
    //add module name
    *this << QString("[" + QString(module) + "]").toLocal8Bit().data();
#endif

    //File, line and function
    *this << QString("[" + QString(file) + ":" + QString::number(line) + ":" + QString(func) + "]").toLocal8Bit().data();

}

SensordLogger::~SensordLogger()
{
    if (printLog) {

#ifdef SENSORD_USE_SYSLOG
        int logPriority;
    
        switch (currentLevel) {
            case SensordLogN:
            default:
            case SensordLogTest:
                logPriority = LOG_DEBUG;
                break;
            case SensordLogDebug:
                logPriority = LOG_INFO;
                break;
            case SensordLogWarning:
                logPriority = LOG_WARNING;
                break;
            case SensordLogCritical:
                logPriority = LOG_CRIT;
                break;
        }
    
        syslog(logPriority, data.toLocal8Bit().data());
        closelog();
#else
        fcntl(STDERR_FILENO, F_SETFL, O_WRONLY);
        QTextStream::operator<<('\n');
        QTextStream(stderr) << data;
#endif
    }

    setDevice(NULL);
}

void SensordLogger::setOutputLevel(SensordLogLevel level)
{
  outputLevel = level;
}


SensordLogLevel SensordLogger::getOutputLevel()
{
  return outputLevel;
}

void SensordLogger::signalHandler(int param) 
{
    Q_UNUSED(param);
    outputLevel = static_cast<SensordLogLevel> ((static_cast<int>(outputLevel) + 1) % SensordLogN);

    std::cerr << "Signal Handled \n";
    std::cerr << "New debugging level: " << outputLevel << "\n";
    //std::cerr << "Debugging output " << (printDebug?"enabled":"disabled") << "\n";
}

void SensordLogger::signalFlush(int param)
{
    Q_UNUSED(param);

    QStringList output;

    output.append("Flushing sensord state\n");
    output.append(QString("  Logging level: %1\n").arg(outputLevel));

    SensorManager& sm = SensorManager::instance();
    output.append("  Adaptors:\n");
    foreach (QString key, sm.deviceAdaptorInstanceMap_.keys()) {
        DeviceAdaptorInstanceEntry& entry = sm.deviceAdaptorInstanceMap_[key];
        output.append(QString("    %1 [%2 listener(s)]\n").arg(entry.type_).arg(entry.cnt_));
    }

    output.append("  Chains:\n");
    foreach (QString key, sm.chainInstanceMap_.keys()) {
        ChainInstanceEntry& entry = sm.chainInstanceMap_[key];
        output.append(QString("    %1 [%2 listener(s)]. %3\n").arg(entry.type_).arg(entry.cnt_).arg(entry.chain_->running()?"Running":"Stopped"));
    }

    output.append("  Logical sensors:\n");
    foreach (QString key, sm.sensorInstanceMap_.keys()) {
        SensorInstanceEntry& entry = sm.sensorInstanceMap_[key];
        bool control = true;
        if (entry.controllingSession_ <= 0) {
            control = false;
        }
        output.append(QString("    %1 [%2 %3 listen session(s)]. %4\n").arg(entry.type_).arg(control? "Control +":"No control,").arg(entry.listenSessions_.size()).arg(entry.sensor_->running()?"Running":"Stopped"));
    }

    foreach (QString line, output) {

#ifdef SENSORD_USE_SYSLOG
        openlog(NULL, LOG_PERROR, LOG_DAEMON);
        syslog(LOG_DEBUG, line.toLocal8Bit().data());
        closelog();
#else
        fcntl(STDERR_FILENO, F_SETFL, O_WRONLY);
        QTextStream(stderr) << line;
#endif
    }
}
