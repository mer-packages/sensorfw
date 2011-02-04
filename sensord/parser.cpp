/**
   @file parser.cpp
   @brief Sensord initiation point

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Lihan Guo <ext-lihan.guo@nokia.com>

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
#include <iostream>

Parser::Parser(QStringList arguments) :
    printHelp_(false),
    contextInfo_(true),
    changeLogLevel_(false),
    configFile_(false),
    daemon_(false),
    magnetometerCalibration_(true),
    configFilePath_(""),
    logLevel_(SensordLogWarning),
    logTarget_(8),
    logFilePath_("/var/log/sensord.log")
{
    parsingCommandLine(arguments);
}

Parser::~Parser()
{
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
        else if (opt.startsWith("--config-dir"))
        {
            data = opt.split("=");
            configDir_ = true;
            configDirPath_ = data.at(1);
        }
        else if (opt.startsWith("--no-context-info"))
            contextInfo_ = false;
        else if (opt.startsWith("--no-magnetometer-bg-calibration"))
            magnetometerCalibration_ = false;
        else if (opt.startsWith("-d") || opt.startsWith("--daemon"))
            daemon_ = true;
        else if (opt.startsWith("-h") || opt.startsWith("--help"))
            printHelp_ = true;
        else if (opt.startsWith("-"))
            std::cerr << "Unknown option: " << opt.toStdString() << std::endl;
    }
}

bool Parser::printHelp() const
{
    return printHelp_;
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

bool Parser::configDirInput() const
{
    return configDir_;
}

const QString& Parser::configDirPath() const
{
    return configDirPath_;
}

bool Parser::contextInfo() const
{
    return contextInfo_;
}

bool Parser::magnetometerCalibration() const
{
    return magnetometerCalibration_;
}

bool Parser::createDaemon() const
{
    return daemon_;
}

int Parser::logTarget() const
{
    return logTarget_;
}

const QString& Parser::logFilePath() const
{
    return logFilePath_;
}
