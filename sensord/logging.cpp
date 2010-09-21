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
#endif
#include <QDateTime>

SensordLogLevel SensordLogger::outputLevel = SensordLogWarning;
bool SensordLogger::initialized = false;
int SensordLogger::m_target=STDERR_FILENO;
QMutex SensordLogger::m_mutex;
QMutexLocker SensordLogger::m_locker(&m_mutex);
QFile* SensordLogger::m_file;

SensordLogger::SensordLogger (const char* func, const char *file, int line, SensordLogLevel level) :
        currentLevel(level)
{
    if ((currentLevel >= SensordLogTest) && (currentLevel < SensordLogN) && (currentLevel >= getOutputLevel()))
        return;

    init();
    oss << logLevelToText(level).toLocal8Bit().data();
    oss << "[" << file << ":" << line << ":" << func << "]: ";
}

SensordLogger::~SensordLogger()
{

    if ((currentLevel >= SensordLogTest) && (currentLevel < SensordLogN) && (currentLevel >= getOutputLevel()))
        return;

#ifdef SENSORD_USE_SYSLOG
    syslog(logPriority(currentLevel), "%s", oss.str().c_str());
#endif

    std::ostringstream prefix;
    prefix << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().data();
    prefix << " [sensord] ";
    prefix << oss.str();
    prefix << '\n';
    oss.str(prefix.str());
    printToTarget(oss.str().c_str());
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
#endif
        initTarget();
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


void SensordLogger::printToTarget(QString data){

    m_locker.relock();

    if ((m_target & STDERR_FILENO) > 0){
        QTextStream(stderr) << data;
    }

    if ((m_target & STDOUT_FILENO) > 0){
        QTextStream(stdout) << data;
    }

    if ((m_target & 4) > 0){
        QTextStream(m_file)<<data;
    }
    m_locker.unlock();
}


void SensordLogger::initTarget(){
    if ((m_target & STDERR_FILENO) > 0){
        fcntl(STDERR_FILENO, F_SETFL, O_WRONLY);
    }

    if ((m_target & STDOUT_FILENO) > 0){
        fcntl(STDOUT_FILENO, F_SETFL, O_WRONLY);
    }

    if ((m_target & 4) > 0){
        if (m_file==0) m_file = new QFile(QString("/var/log/sensord.log"));
        if (!m_file->isOpen())  m_file->open(QIODevice::WriteOnly | QIODevice::Text);
    }
}


int SensordLogger::logPriority(int currentLevel){
#ifdef SENSORD_USE_SYSLOG
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
#endif
}

QString SensordLogger::logLevelToText(int level){
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
