/**
   @file sensorhandler_qmsystem2.cpp
   @brief sensor handler
   <p>
   Copyright (C) 2011 Nokia Corporation

   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>

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

#include "sensorhandler_qtmob.h"
#include "logging.h"

SensorHandler::SensorHandler(const QString& sensorName, QObject *parent) :
    AbstractSensorHandler(sensorName, parent),
    m_sensor(NULL)
{
    if (sensorName_ == "compasssensor")
    {
        m_sensor = new QCompass();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedCompassData()));
    } 
    else if (sensorName_ == "magnetometersensor")
    {
        m_sensor = new QMagnetometer();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedMagnetometerData()));
    }
    else if (sensorName_ ==  "orientationsensor")
    {
        m_sensor = new QOrientationSensor();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedOrientationData()));
    }
    else if (sensorName_ == "accelerometersensor")
    {
        m_sensor = new QAccelerometer();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedAccelerometerData()));
    }
    else if (sensorName_ == "alssensor")
    {
        m_sensor = new QAmbientLightSensor();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedAlsData()));
    }
    else if (sensorName_ == "rotationsensor")
    {
        m_sensor = new QRotationSensor();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedRotationData()));
    }
    else if (sensorName_ == "tapsensor")
    {
        m_sensor = new QTapSensor();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedTapData()));
    }
    else if (sensorName_ == "proximitysensor")
    {
        m_sensor = new QProximitySensor();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedProximityData()));
    }

    m_sensor -> setProperty("alwaysOn", standbyoverride_);
    m_sensor -> setProperty("bufferSize", buffersize_);
}

SensorHandler::~SensorHandler()
{
    delete m_sensor;
}

void SensorHandler::receivedAccelerometerData()
{
    QAccelerometerReading* data = (QAccelerometerReading*) m_sensor->reading();
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "x " << data->x() << "y " <<  data->y() << "z " <<  data->z();
}

void SensorHandler::receivedCompassData()
{
    QCompassReading* data = (QCompassReading*) m_sensor->reading();
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
             << " azimuth: " << data->azimuth() << "calibrationLevel: " <<  data-> calibrationLevel();

}

void SensorHandler::receivedAlsData()
{
    QAmbientLightReading* data = (QAmbientLightReading*) m_sensor->reading();
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "lightLevel  " << data->lightLevel();
}

void SensorHandler::receivedMagnetometerData()
{
    QMagnetometerReading* data = (QMagnetometerReading*) m_sensor->reading();
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "x " << data->x() << "y " <<  data->y() << "z " <<  data->z() 
                  << " calibrationLevel" << data->calibrationLevel();
}

void SensorHandler::receivedOrientationData()
{
    QOrientationReading* data = (QOrientationReading*) m_sensor->reading();
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "orientation " << data->orientation();
}

void SensorHandler::receivedRotationData()
{
    QRotationReading* data = (QRotationReading*) m_sensor->reading();
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "x " << data->x() << "y " <<  data->y() << "z " <<  data->z();
}

void SensorHandler::receivedTapData()
{
    QTapReading* data = (QTapReading*) m_sensor->reading();
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "DoubleTap " <<  data->isDoubleTap();
}

void SensorHandler::receivedProximityData()
{
    QProximityReading* data = (QProximityReading*) m_sensor->reading();
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "Close " <<  data->close();
}

bool SensorHandler::startClient()
{
    if (m_sensor -> start())
        return true;
    return false;
}

void SensorHandler::init(const QStringList&)
{
}

bool SensorHandler::stopClient()
{
    m_sensor -> stop();
    return true;
}
