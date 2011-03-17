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

#include "sensorhandler_qmsystem2.h"
#include "logging.h"

SensorHandler::SensorHandler(const QString& sensorName, QObject *parent) :
    AbstractSensorHandler(sensorName, parent),
    sensor_(NULL)
{
    if (sensorName_ == "compasssensor")
    {
        sensor_ = new MeeGo::QmCompass();
        connect(sensor_, SIGNAL(dataAvailable(const MeeGo::QmCompassReading)), this, SLOT(receivedData(const MeeGo::QmCompassReading)));
    }
    else if (sensorName_ == "magnetometersensor")
    {
        sensor_ = new MeeGo::QmMagnetometer();
        connect(sensor_, SIGNAL(dataAvailable(const MeeGo::QmMagnetometerReading&)), this, SLOT(receivedData(const MeeGo::QmMagnetometerReading&)));
    }
    else if (sensorName_ ==  "orientationsensor")
    {
        sensor_ = new MeeGo::QmOrientation();
        connect(sensor_, SIGNAL(orientationChanged(const MeeGo::QmOrientationReading)), this, SLOT(receivedData(const MeeGo::QmOrientationReading)));
    }
    else if (sensorName_ == "accelerometersensor")
    {
        sensor_ = new MeeGo::QmAccelerometer();
        connect(sensor_, SIGNAL(dataAvailable(const MeeGo::QmAccelerometerReading&)), this, SLOT(receivedData(const MeeGo::QmAccelerometerReading&)));
    }
    else if (sensorName_ == "alssensor")
    {
        sensor_ = new MeeGo::QmALS();
        connect(sensor_, SIGNAL(ALSChanged(const MeeGo::QmAlsReading)), this, SLOT(receivedData(const MeeGo::QmIntReading)));
    }
    else if (sensorName_ == "rotationsensor")
    {
        sensor_ = new MeeGo::QmRotation();
        connect(sensor_, SIGNAL(dataAvailable(const MeeGo::QmRotationReading&)), this, SLOT(receivedData(const MeeGo::QmRotationReading&)));
    }
    else if (sensorName_ == "tapsensor")
    {
        sensor_ = new MeeGo::QmTap();
        connect(sensor_, SIGNAL(tapped(const MeeGo::QmTapReading)), this, SLOT(receivedData(const MeeGo::QmTapReading)));
    }
    else if (sensorName_ == "proximitysensor")
    {
        sensor_ = new MeeGo::QmProximity();
        connect(sensor_, SIGNAL(ProximityChanged(const MeeGo::QmProximityReading)), this, SLOT(receivedData(const MeeGo::QmIntReading)));
    }
}

void SensorHandler::receivedData(const MeeGo::QmAccelerometerReading& data)
{
    ++dataCount_;
    sensordLogT() << sensorName_ << " sample " << dataCount_ << ": "
                  << data.x << " " << data.y << " " <<   data.z;
}

void SensorHandler::receivedData(const MeeGo::QmIntReading data)
{
    ++dataCount_;
    sensordLogT() << sensorName_ << " sample " << dataCount_ << ": "
                  << data.value;
}

void SensorHandler::receivedData(const MeeGo::QmCompassReading data)
{
    ++dataCount_;
    sensordLogT() << sensorName_ << " sample " << dataCount_ << ": "
                  << data.level << " " << data.degrees;
}

void SensorHandler::receivedData(const MeeGo::QmMagnetometerReading& data)
{
    ++dataCount_;
    sensordLogT() << sensorName_ << " sample " << dataCount_ << ": "
                  << data.x << " " << data.y << " " << data.z << " "
                  << data.rx << " " << data.ry << " " << data.rz << " "
                  << data.level;
}

void SensorHandler::receivedData(const MeeGo::QmOrientationReading data)
{
    ++dataCount_;
    sensordLogT() << sensorName_ << " sample " << dataCount_ << ": "
                  << data.value;
}

void SensorHandler::receivedData(const MeeGo::QmRotationReading& data)
{
    ++dataCount_;
    sensordLogT() << sensorName_ << " sample " << dataCount_ << ": "
                  << data.x << " " << data.y << " " <<   data.z;
}

void SensorHandler::receivedData(const MeeGo::QmTapReading data)
{
    ++dataCount_;
    sensordLogT() << sensorName_ << " sample " << dataCount_ << ": "
                  << data.type << " " << data.direction;
}

bool SensorHandler::startClient()
{
    sensor_->requestSession(MeeGo::QmSensor::SessionTypeListen);
    sensor_->setInterval(interval_);
    sensor_->setStandbyOverride(standbyoverride_);
    return true;
}

bool SensorHandler::init(const QStringList&)
{
    return true;
}

bool SensorHandler::stopClient()
{
    return true;
}
