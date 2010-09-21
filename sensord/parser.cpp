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

Parser::Parser(QStringList arguments) :
    printHelp_(false),
    contextInfo_(true),
    changeLogLevel_(false),
    configFile_(false),
    daemon_(false),
    magnetometerCalibration_(true),
    configFilePath_(""),
    logLevel_("")
{
    parsingCommandLine(arguments);
}

Parser::~Parser()
{
}

void Parser::parsingCommandLine(QStringList arguments)
{
    foreach (QString opt, arguments)
    {
        QStringList data;

        if (opt.contains("-l=")||opt.contains("--log-level"))
        {
            data = opt.split("=");
            changeLogLevel_ = true;
            logLevel_ = data.at(1);
        }

        if (opt.contains("--log-target"))
        {
            data = opt.split("=");
            logTarget_= data.at(1).toInt();
        }

        if (opt.contains("-c=")||opt.contains("--config-file"))
        {
            data = opt.split("=");
            configFile_ = true;
            configFilePath_ = data.at(1);
        }

        if (opt.contains("--no-context-info"))
            contextInfo_ = false;


        if (opt.contains("--no-magnetometer-bg-calibration"))
            magnetometerCalibration_ = false;


        if (opt.contains("-d")||opt.contains("--daemon"))
            daemon_ = true;

        if (opt.contains("-h")||opt.contains("--help"))
            printHelp_ = true;

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
    if (logLevel_ == "test")
        return SensordLogTest;
    else if (logLevel_ == "debug")
        return SensordLogDebug;
    else if (logLevel_ == "warning")
        return SensordLogWarning;
    else if (logLevel_ == "critical")
        return SensordLogCritical;
    else
        return SensordLogWarning;
}

bool Parser::configFileInput() const
{
    return configFile_;
}

const QString& Parser::configFilePath() const
{
    return configFilePath_;
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
