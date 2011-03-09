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
#include <QDebug>

SensorHandler::SensorHandler(const QString& sensorName, QObject *parent) :
    QThread(parent),
    sensorName_(sensorName),
    interval_(100),
    bufferinterval_(0),
    standbyoverride_(false),
    buffersize_(0),
    dataCount_(0)
{

    if (sensorName_ == "compasssensor")
    {
        m_sensor = new QCompass();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedData(m_sensor->reading())));
    } 
    else if (sensorName_ == "magnetometersensor")
    {
        m_sensor = new QMagnetometer();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedData(m_sensor->reading())));
    }
    else if (sensorName_ ==  "orientationsensor")
    {
        m_sensor = new QOrientationSensor();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedData(m_sensor->reading())));
    }
    else if (sensorName_ == "accelerometersensor")
    {
        m_sensor = new QAccelerometer();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedData(m_sensor->reading())));
    }
    else if (sensorName_ == "alssensor")
    {
        m_sensor = new QAmbientLightSensor();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedData(m_sensor->reading())));
    }
    else if (sensorName_ == "rotationsensor")
    {
        m_sensor = new QRotationSensor();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedData(m_sensor->reading())));
    }
    else if (sensorName_ == "tapsensor")
    {
        m_sensor = new QTapSensor();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedData(m_sensor->reading())));
    }
    else if (sensorName_ == "proximitysensor")
    {
        m_sensor = new QProximitySensor();
        connect(m_sensor, SIGNAL(readingChanged()), 
                this, SLOT(receivedData(m_sensor->reading())));
    }


    if (Config::configuration() != NULL)
    {
        interval_ = Config::configuration()->value(sensorName_ + "/interval", "100").toInt();
        bufferinterval_ = Config::configuration()->value(sensorName_ + "/bufferinterval", "0").toInt();
        standbyoverride_ = Config::configuration()->value(sensorName_ + "/standbyoverride", "false").toBool();
        buffersize_ = Config::configuration()->value(sensorName_ + "/buffersize", "0").toInt();

    }
   
    m_sensor -> connectToBackend();
    m_sensor -> setProperty("alwaysOn", true);
    m_sensor -> setProperty("bufferSize", buffersize_);
    m_sensor -> setDataRate(interval_);
}

void SensorHandler::receivedData(const QAccelerometerReading* data)
{
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "x " << data->x() << "y " <<  data->y() << "z " <<  data->z();
}

void SensorHandler::receivedData(const QCompassReading* data)
{
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
             << " azimuth: " << data->azimuth() << "calibrationLevel: " <<  data-> calibrationLevel();

}

void SensorHandler::receivedData(const QAmbientLightReading* data)
{
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "lightLevel  " << data->lightLevel();
}

void SensorHandler::receivedData(const QMagnetometerReading* data)
{
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "x " << data->x() << "y " <<  data->y() << "z " <<  data->z() 
                  << " calibrationLevel" << data->calibrationLevel();
}

void SensorHandler::receivedData(const QOrientationReading* data)
{
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "orientation " << data->orientation();
}

void SensorHandler::receivedData(const QRotationReading* data)
{
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "x " << data->x() << "y " <<  data->y() << "z " <<  data->z();
}

void SensorHandler::receivedData(const QTapReading* data)
{
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "DoubleTap " <<  data->isDoubleTap();
}

void SensorHandler::receivedData(const QProximityReading* data)
{
    ++dataCount_;
    sensordLogT() << (this-> sensorName_) << " sample " << dataCount_ << ": " 
                  << "Close " <<  data->close();
}


bool SensorHandler::startClient()
{
    if (m_sensor -> start()){
        m_sensor -> setProperty("alwaysOn", true);
        m_sensor -> setProperty("bufferSize", buffersize_);
        m_sensor -> setDataRate(interval_);
        return true;
    }
    return false;
}

void SensorHandler::init(const QStringList& sensors)
{
}


void SensorHandler::stopClient()
{
    m_sensor -> stop();
}
