/**
   @file sensorhandler_qmsystem2.cpp
   @brief sensor handler
   <p>
   Copyright (C) 2011 Nokia Corporation

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

#include "sensorhandler.h"
#include "logging.h"

SensorHandler::SensorHandler(const QString& sensorName, QObject *parent) :
    QThread(parent),
    sensorName_(sensorName),
    interval_(100),
    bufferinterval_(0),
    standbyoverride_(false),
    buffersize_(0),
    dataCount_(0),
    frameCount_(0)
{
    if (sensorName_ == "compasssensor")
    {
    }
    else if (sensorName_ == "magnetometersensor")
    {
    }
    else if (sensorName_ ==  "orientationsensor")
    {
    }
    else if (sensorName_ == "accelerometersensor")
    {
    }
    else if (sensorName_ == "alssensor")
    {
    }
    else if (sensorName_ == "rotationsensor")
    {
    }
    else if (sensorName_ == "tapsensor")
    {
    }
    else if (sensorName_ == "proximitysensor")
    {
    }

    if (Config::configuration() != NULL)
    {
        interval_ = Config::configuration()->value(sensorName_ + "/interval", "100").toInt();
        bufferinterval_ = Config::configuration()->value(sensorName_ + "/bufferinterval", "0").toInt();
        standbyoverride_ = Config::configuration()->value(sensorName_ + "/standbyoverride", "false").toBool();
        buffersize_ = Config::configuration()->value(sensorName_ + "/buffersize", "0").toInt();

    }
}

void SensorHandler::startClient()
{
}

void SensorHandler::init(const QStringList& sensors)
{
}
