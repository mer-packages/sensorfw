/**
   @file parser.cpp
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

#include "parser.h"
#include <QDebug>
#include <iostream>

Parser::Parser(QStringList arguments) :
    printHelp_(false),
    changeLogLevel_(false),
    configFile_(false),
    singleThread_(true),
    gracefulShutdown_(true),
    configFilePath_(""),
    logLevel_(SensordLogWarning),
    logTarget_(1),
    logFilePath_("/var/log/sensors-testapp.log"),
    statInterval_(5000)
{
    parsingCommandLine(arguments);
}

void Parser::parsingCommandLine(QStringList arguments)
{
    foreach (const QString& arg, arguments)
    {
        QStringList data;
        QString opt = arg.trimmed();

        if (opt.startsWith("-l=") || opt.startsWith("--log-level"))
        {
            data = opt.split("=");
            changeLogLevel_ = true;
            QString logLevel = data.at(1);
            if (logLevel == "test")
                logLevel_ = SensordLogTest;
            else if (logLevel == "debug")
                logLevel_ = SensordLogDebug;
            else if (logLevel == "warning")
                logLevel_ = SensordLogWarning;
            else if (logLevel == "critical")
                logLevel_ = SensordLogCritical;
            else
                logLevel_ = SensordLogWarning;
        }
        else if (opt.startsWith("-i=") || opt.startsWith("--stat-interval"))
        {
            data = opt.split("=");
            statInterval_ = data.at(1).toInt();
        }
        else if (opt.startsWith("--log-target"))
        {
            data = opt.split("=");
            logTarget_= data.at(1).toInt();
        }
        else if (opt.startsWith("--log-file-path"))
        {
            data = opt.split("=");
            logFilePath_ = data.at(1);
        }
        else if (opt.startsWith("-c=") || opt.startsWith("--config-file"))
        {
            data = opt.split("=");
            configFile_ = true;
            configFilePath_ = data.at(1);
        }
        else if (opt.startsWith("-m="))
        {
            data = opt.split("=");
            if (data.at(1).toInt() == 2)
            {
                singleThread_ = false ;
            }
        }
        else if (opt.startsWith("-g="))
        {
            data = opt.split("=");
            gracefulShutdown_ = !data.at(1).toInt() == 0;
        }
        else if (opt.startsWith("-h") || opt.startsWith("--help"))
            printHelp_ = true;
        else if (opt.startsWith("-"))
            std::cerr << "Unknown option: " << opt.toStdString() << std::endl;
    }
}

bool Parser::changeLogLevel() const
{
    return changeLogLevel_;
}

SensordLogLevel Parser::getLogLevel() const
{
    return logLevel_;
}

bool Parser::configFileInput() const
{
    return configFile_;
}

const QString& Parser::configFilePath() const
{
    return configFilePath_;
}

bool Parser::singleThread() const
{
    return singleThread_;
}

int Parser::logTarget() const
{
    return logTarget_;
}

const QString& Parser::logFilePath() const
{
    return logFilePath_;
}

bool Parser::printHelp() const
{
    return printHelp_;
}

int Parser::statInterval() const
{
    return statInterval_;
}

bool Parser::gracefulShutdown() const
{
    return gracefulShutdown_;
}
