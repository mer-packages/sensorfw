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

#ifdef SENSORD_USE_SYSLOG
#include <syslog.h>
#else
#include <QDateTime>
#endif

SensordLogLevel SensordLogger::outputLevel = SensordLogWarning;
bool SensordLogger::initialized = false;

SensordLogger::SensordLogger (const char* func, const char *file, int line, SensordLogLevel level) :
    currentLevel(level)
{
    init();

    switch (level) {
        case SensordLogTest:
            oss << "*TEST* ";
            break;
        case SensordLogDebug:
            oss << "*DEBUG* ";
            break;
        case SensordLogWarning:
            oss << "*WARNING* ";
            break;
        case SensordLogCritical:
            oss << "*CRITICAL* ";
            break;
        case SensordLogN:
        default:
            break;
    }
    oss << "[" << file << ":" << line << ":" << func << "]: ";
}

SensordLogger::~SensordLogger()
{
#ifndef SENSORD_USE_SYSLOG
    std::ostringstream prefix;
    prefix << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data();
    prefix << " [sensord] ";
    prefix << oss.str();
    oss.str(prefix.str());
#endif

    if ((currentLevel >= SensordLogTest) && (currentLevel < SensordLogN) && (currentLevel >= outputLevel))
    {
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

        syslog(logPriority, "%s", oss.str().c_str());
#else
        std::cerr << oss.str() << '\n';
#endif
    }
}

void SensordLogger::setOutputLevel(SensordLogLevel level)
{
    outputLevel = level;
}

SensordLogLevel SensordLogger::getOutputLevel()
{
    return outputLevel;
}

void SensordLogger::init()
{
    if(!initialized)
    {
#ifdef SENSORD_USE_SYSLOG
        openlog("sensord", LOG_PID, LOG_DAEMON);
#else
        fcntl(STDERR_FILENO, F_SETFL, O_WRONLY);
#endif
        initialized = true;
    }
}

void SensordLogger::close()
{
    if(initialized)
    {
#ifdef SENSORD_USE_SYSLOG
        closelog();
#endif
        initialized = false;
    }
}
