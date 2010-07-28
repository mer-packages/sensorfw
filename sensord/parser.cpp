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


Parser::Parser(QStringList &arguments)
{
    
    contextInfo_ = true;
    magnetometerCalibration_ = true;

    printHelp_ = false;
    changeLogLevel_ = false;
    configFile_ = false;
    daemon_ = false;

    configFilePath_ = "";
    logLevel_ = "";
       
    parsingCommandLine(arguments); 

}


Parser::~Parser()
{
} 


void Parser::parsingCommandLine(QStringList &arguments)
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


bool Parser::printHelp()
{
    return printHelp_;
}


bool Parser::changeLogLevel()
{
    return changeLogLevel_;
}


SensordLogLevel Parser:: getLogLevel()
{

    if (logLevel_.operator==("test")) 
        return SensordLogTest;
    if (logLevel_.operator==("debug"))
        return SensordLogDebug;
    if (logLevel_.operator==("warning")) 
        return SensordLogWarning;
    if (logLevel_.operator==("critical")) 
        return SensordLogCritical;
}


bool Parser:: configFileInput()
{
    return configFile_;
}


QString Parser:: configFilePath()  
{
    return configFilePath_;  
}

    
bool Parser::contextInfo()
{
    return contextInfo_;
}


bool Parser::magnetometerCalibration()
{
    return magnetometerCalibration_; 
}


bool Parser::createDaemon()
{
    return daemon_;
}
