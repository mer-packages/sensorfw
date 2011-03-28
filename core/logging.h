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
#include <fstream>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QMutex>

/**
 * Log levels
 */
enum SensordLogLevel
{
    SensordLogTest = 1, /**< Test */
    SensordLogDebug,    /**< Debug */
    SensordLogWarning,  /**< Warning */
    SensordLogCritical, /**< Critical */
    SensordLogN
};

/**
 * Logging utility. All sensorfw logging is done using this class.
 * Set of macro functions exists for convenient usage.
 */
class SensordLogger
{
public:
    /**
     * Constructor.
     *
     * @param func Function name where logger is used.
     * @param file File name where logger is used.
     * @param line Line number of the file where logger is used.
     * @param level Logging verbosity level.
     */
    SensordLogger(const char *func, const char *file, int line, SensordLogLevel level);

    /**
     * Destructor.
     */
    ~SensordLogger();

    /**
     * Stream given parameter into logger.
     *
     * @param item item to be logged.
     * @return logger.
     */
    SensordLogger& operator<<(const QStringList& item)
    {
        if(initialized && oss && isLoggable(currentLevel))
            return this->operator<<(item.join(", "));
        return *this;
    }

    /**
     * Stream given parameter into logger.
     *
     * @param item item to be logged.
     * @return logger.
     */
    SensordLogger& operator<<(const QString& item)
    {
        if(initialized && oss && isLoggable(currentLevel))
            *oss << item.toStdString();
        return *this;
    }

    /**
     * Stream given parameter into logger.
     *
     * @tparam T item type to be logged.
     * @param item item to be logged.
     * @return logger.
     */
    template <typename T>
    SensordLogger& operator<<(const T& item)
    {
        if(initialized && oss && isLoggable(currentLevel))
            *oss << item;
        return *this;
    }

    /**
     * Configure global output level for the logger.
     *
     * @param level Only messages with this or higher level will be logged.
     */
    static void setOutputLevel(SensordLogLevel level);

    /**
     * Get global output level for the logger.
     *
     * @return global output level.
     */
    static inline SensordLogLevel getOutputLevel() { return outputLevel; }

    /**
     * Initialize logger. This must be called once in the application
     * lifetime before using logger.
     *
     * @param target output target bits for logger. Bits are:
     *                 1 = stdout
     *                 2 = stderr
     *                 4 = log file
     *                 8 = syslog
     * @param logFilePath when target (4) is enabled this is the
     *                    log file path.
     * @param name        application name to be logged.
     */
    static void init(int target, const QString& logFilePath, const QString& name);

    /**
     * Close logger. This should be called before exiting application
     * when logger is not needed anymore.
     */
    static void close();

private:
    std::ostringstream* oss;             /**< log buffer */
    SensordLogLevel currentLevel;        /**< level for current message */

    static SensordLogLevel outputLevel;  /**< logging output level */
    static bool initialized;             /**< is logger initialized */
    static std::ofstream* logFile;       /**< log file path */
    static QMutex mutex;                 /**< logger mutex */
    static int logTarget;                /**< log target */
    static QByteArray appName;           /**< application name */

    /**
     * Print given data to the configured log targets.
     *
     * @param data text to be logged.
     */
    void printToTarget(const char* data) const;

    /**
     * Convert log level into syslog priority.
     *
     * @param currentLevel log level.
     * @return syslog priority.
     */
    static int logPriority(int currentLevel);

    /**
     * Convert log level into text.
     *
     * @param level log level.
     * @return log level in textual form.
     */
    static const char* logLevelToText(int level);

    /**
     * Is given level loggable with current settings.
     *
     * @param level log level.
     * @return is level loggable with current settings.
     */
    inline static bool isLoggable(int level)
    {
        return ((level >= SensordLogTest) && (level < SensordLogN) && (level >= getOutputLevel()));
    }
};

#define sensordLogT() (SensordLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogTest))
#define sensordLogD() (SensordLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogDebug))
#define sensordLogW() (SensordLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogWarning))
#define sensordLogC() (SensordLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogCritical))
#define sensordLog() (SensordLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__, SensordLogTest))

#endif //LOGGING_H
