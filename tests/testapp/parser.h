/**
   @file parser.h
   @brief parsing command parameter

   <p>
   Copyright (C) 2011 Nokia Corporation

   @author Shenghua <shenghua.1.liu@nokia.com>

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

#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include "logging.h"

class Parser
{
public:

    Parser(QStringList arguments);

    void parsingCommandLine(QStringList args);

    bool printHelp() const;

    bool changeLogLevel() const;
    SensordLogLevel getLogLevel() const;

    bool configFileInput() const;
    const QString& configFilePath() const;

    bool singleThread() const;
    bool gracefulShutdown() const;

    int logTarget() const;
    const QString& logFilePath() const;

    int statInterval() const;

private:
    bool printHelp_;
    bool changeLogLevel_;
    bool configFile_;
    bool singleThread_;
    bool gracefulShutdown_;

    QString configFilePath_;
    SensordLogLevel logLevel_;
    int logTarget_;
    QString logFilePath_;
    int statInterval_;
};

#endif // PARSER_H
