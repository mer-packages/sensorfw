/**
   @file abstractsensorhandler.cpp
   @brief abstract sensor handler

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

#include "abstractsensorhandler.h"
#include "config.h"
#include "logging.h"

AbstractSensorHandler::AbstractSensorHandler(const QString& sensorName, QObject *parent) :
    QThread(parent),
    sensorName_(sensorName),
    interval_(100),
    bufferinterval_(0),
    standbyoverride_(false),
    buffersize_(0),
    dataCount_(0),
    frameCount_(0),
    downsample_(false)
{
    if (Config::configuration() != NULL)
    {
        interval_ = Config::configuration()->value(sensorName_ + "/interval", 100);
        bufferinterval_ = Config::configuration()->value(sensorName_ + "/bufferinterval", 0);
        standbyoverride_ = Config::configuration()->value(sensorName_ + "/standbyoverride", false);
        buffersize_ = Config::configuration()->value(sensorName_ + "/buffersize", 0);
        downsample_ = Config::configuration()->value(sensorName_ + "/downsample", false);
    }
}

AbstractSensorHandler::~AbstractSensorHandler()
{
}

void AbstractSensorHandler::run()
{
    startClient();
    exec();
}
