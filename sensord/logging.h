/**
   @file logging.h
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

#ifndef LOGGING_H
#define LOGGING_H

#include <sstream>
#include <QString>
#include <QStringList>

enum SensordLogLevel {
    SensordLogTest = 1,
    SensordLogDebug,
    SensordLogWarning,
    SensordLogCritical,
    SensordLogN
};

class SensordLogger
{
public:
    SensordLogger(const char *func, const char *file, int line, SensordLogLevel level);
    ~SensordLogger();

    SensordLogger& operator<<(const QStringList& item)
    {
        return this->operator<<(item.join(", "));
    }

    SensordLogger& operator<<(const QString& item)
    {
        oss << item.data();
        return *this;
    }

    template <typename T>
    SensordLogger& operator<<(const T& item)
    {
        oss << item;
        return *this;
    }

private:
    std::ostringstream oss;
    SensordLogLevel currentLevel;

    static SensordLogLevel outputLevel;
    static bool initialized;

public:

    static void setOutputLevel(SensordLogLevel level);
    static SensordLogLevel getOutputLevel();
    static void init();
    static void close();
};

#define sensordLogT() (SensordLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogTest))
#define sensordLogD() (SensordLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogDebug))
#define sensordLogW() (SensordLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogWarning))
#define sensordLogC() (SensordLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogCritical))
#define sensordLog() (SensordLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogTest))

#endif //LOGGING_H
