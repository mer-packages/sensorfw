/**
   @file logging.h
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

#ifndef LOGGING_H
#define LOGGING_H

#include <QTextStream>
#include <QString>
#include <QDebug>
#include <QBuffer>
#include <QIODevice>

enum SensordLogLevel {
    SensordLogTest = 1,
    SensordLogDebug,
    SensordLogWarning,
    SensordLogCritical,

    SensordLogN
};

class SensordLogger : public QTextStream
{
public:
    SensordLogger(const char *module, const char *func, const char *file, int line, SensordLogLevel level);
    ~SensordLogger();

    template <typename T> SensordLogger &operator<< (const T& item) 
    {
        QString out;
        QDebug(&out) << item;
        QTextStream::operator<<(out);
        return *this;
    }
private:
    static SensordLogLevel outputLevel;
    bool printLog;
    static void signalHandler(int param);
    static void signalFlush(int param);

    const char* moduleName;
    SensordLogLevel currentLevel;
    QString data;
};

#define sensordLogT() (SensordLogger("Sensord", __PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogTest))
#define sensordLogD() (SensordLogger("Sensord", __PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogDebug))
#define sensordLogW() (SensordLogger("Sensord", __PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogWarning))
#define sensordLogC() (SensordLogger("Sensord", __PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogCritical))

#define sensordLog() (SensordLogger("Sensord", __PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogTest))

#endif //LOGGING_H
