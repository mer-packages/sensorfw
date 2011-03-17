/**
   @file logging.cpp
   @brief Syslog utility

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#include "logging.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <QMutexLocker>
#include <syslog.h>
#include <QDateTime>

SensordLogLevel SensordLogger::outputLevel = SensordLogWarning;
bool SensordLogger::initialized = false;
int SensordLogger::logTarget = 0;
QMutex SensordLogger::mutex;
std::ofstream* SensordLogger::logFile = 0;
QByteArray SensordLogger::appName = "";

SensordLogger::SensordLogger (const char* func, const char *file, int line, SensordLogLevel level) :
    oss(0),
    currentLevel(level)
{
    if (!initialized || !isLoggable(level))
        return;
    oss = new std::ostringstream();
    *oss << logLevelToText(level);
    *oss << "[" << file << ":" << line << ":" << func << "]: ";
}

SensordLogger::~SensordLogger()
{
    if (!initialized || !isLoggable(currentLevel))
        return;

    printToTarget(oss->str().c_str());
    delete oss;
}

void SensordLogger::setOutputLevel(SensordLogLevel level)
{
    outputLevel = level;
}

void SensordLogger::close()
{
    if (initialized)
    {
        closelog();
        delete logFile;
        logFile = 0;
        initialized = false;
    }
}

void SensordLogger::printToTarget(const char* data) const
{
    if (logTarget == 0)
        return;
    QMutexLocker locker(&mutex);
    std::ostringstream fdLogData;

    if (logTarget & STDERR_FILENO ||
        logTarget & STDOUT_FILENO ||
        logTarget & 4) {
        fdLogData << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data();
        fdLogData << " [" << appName.constData() << "] ";
        fdLogData << data;
    }
    if (logTarget & STDERR_FILENO) {
        std::cerr << fdLogData.str().c_str() << std::endl;
    }

    if (logTarget & STDOUT_FILENO) {
        std::cout << fdLogData.str().c_str() << std::endl;
    }

    if (logTarget & 4) {
        if(logFile)
            *logFile << fdLogData.str().c_str() << std::endl;
    }

    if (logTarget & 8) {
        syslog(logPriority(currentLevel), "%s", data);
    }
}

void SensordLogger::init(int target, const QString& logFilePath, const QString& name)
{
    if (initialized)
        return;
    logTarget = target;
    appName = name.toAscii();
    if (target & STDERR_FILENO){
        fcntl(STDERR_FILENO, F_SETFL, O_WRONLY);
    }

    if (target & STDOUT_FILENO){
        fcntl(STDOUT_FILENO, F_SETFL, O_WRONLY);
    }

    if (target & 4){
        logFile = new std::ofstream(logFilePath.toStdString().c_str(), std::ios::out | std::ios::app);
        if(!logFile->is_open())
            delete logFile;
    }

    if (target & 8){
        openlog(appName, LOG_PID, LOG_DAEMON);
    }
    initialized = true;
}

int SensordLogger::logPriority(int currentLevel)
{
    switch (currentLevel) {
        case SensordLogN:
        default:
        case SensordLogTest:
            return LOG_DEBUG;
        case SensordLogDebug:
            return LOG_INFO;
        case SensordLogWarning:
            return LOG_WARNING;
        case SensordLogCritical:
            return LOG_CRIT;
    }
}

const char* SensordLogger::logLevelToText(int level)
{
    switch (level) {
        case SensordLogTest:
            return "*TEST* ";
        case SensordLogDebug:
            return "*DEBUG* ";
        case SensordLogWarning:
            return "*WARNING* ";
        case SensordLogCritical:
            return "*CRITICAL* ";
        case SensordLogN:
        default:
            return "";
    }
}
