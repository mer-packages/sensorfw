/**
   @file sensorhandler.cpp
   @brief sensor handler
   <p>
   Copyright (C) 2010-2011 Nokia Corporation

   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
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
    sensorChannelInterface_(NULL),
    interval_(100),
    bufferinterval_(0),
    standbyoverride_(false),
    buffersize_(0),
    dataCount_(0),
    frameCount_(0)
{
    if (Config::configuration() != NULL)
    {
        interval_ = Config::configuration()->value(sensorName_ + "/interval", "100").toInt();
        bufferinterval_ = Config::configuration()->value(sensorName_ + "/bufferinterval", "0").toInt();
        standbyoverride_ = Config::configuration()->value(sensorName_ + "/standbyoverride", "false").toBool();
        buffersize_ = Config::configuration()->value(sensorName_ + "/buffersize", "0").toInt();

    }
}

void SensorHandler::receivedData(const MagneticField& data)
{
    dataCount_++;
    sensordLogT() << this->objectName() << " sample " << dataCount_ << ": "
                  << data.x() << " " << data.y() << " " <<   data.z();
}

void SensorHandler::receivedData(const XYZ& data)
{
    dataCount_++;
    sensordLogT() << this->objectName() << " sample " << dataCount_ << ": "
                  << data.x() << " " << data.y() << " " <<   data.z();
}

void SensorHandler::receivedData(const Compass& data)
{
    dataCount_++;
    sensordLogT() << this->objectName() << " sample " << dataCount_ << ": "
                  << data.degrees() << " " << data.level();
}

void SensorHandler::receivedData(const Unsigned& data)
{
    dataCount_++;
    sensordLogT() << this->objectName() << " sample " << dataCount_ << ": "
                  << data.x();
}

void SensorHandler::receivedData(const Tap& data)
{
    dataCount_++;
    sensordLogT() << this->objectName() << " sample " << dataCount_ << ": "
                  << data.direction() << " " << data.type();
}

void SensorHandler::receivedFrame(const QVector<MagneticField>& frame)
{
    frameCount_++;
    sensordLogT() << this->objectName() << " frame " << frameCount_ << " size " << frame.size();
    foreach (const MagneticField& data, frame)
        sensordLogT() << data.x() << " " << data.y() << " " << data.z();
}

void SensorHandler::receivedFrame(const QVector<XYZ>& frame)
{
    frameCount_++;
    sensordLogT() << this->objectName() << " frame " << frameCount_ << " size " << frame.size();
    foreach (const XYZ& data, frame)
        sensordLogT() << data.x() << " " << data.y() << " " << data.z();
}

void SensorHandler::startClient()
{
    createSensorInterface();
    if (sensorChannelInterface_ == 0)
    {
         sensordLogD() << "Creating sensor client interface fails.";
         return;
    }
    sensorChannelInterface_->setInterval(interval_);
    sensorChannelInterface_->setBufferInterval(bufferinterval_);
    sensorChannelInterface_->setBufferSize(buffersize_);
    sensorChannelInterface_->setStandbyOverride(standbyoverride_);
    sensorChannelInterface_->start();
}

void SensorHandler::run()
{

    startClient();

}

void SensorHandler::createSensorInterface()
{
    if (sensorName_ == "compasssensor")
    {
        sensorChannelInterface_ = CompassSensorChannelInterface::interface("compasssensor");
        connect(sensorChannelInterface_, SIGNAL(dataAvailable(const Compass&)), this, SLOT(receivedData(const Compass&)));
    }
    else if (sensorName_ == "magnetometersensor")
    {
        sensorChannelInterface_ = MagnetometerSensorChannelInterface::interface("magnetometersensor");
        connect(sensorChannelInterface_, SIGNAL(dataAvailable(const MagneticField&)), this, SLOT(receivedData(const MagneticField&)));
        connect(sensorChannelInterface_, SIGNAL(frameAvailable(const QVector<MagneticField>&)), this, SLOT(receivedFrame(const QVector<MagneticField>&)));
    }
    else if (sensorName_ ==  "orientationsensor")
    {
        sensorChannelInterface_ = OrientationSensorChannelInterface::interface("orientationsensor");
        connect(sensorChannelInterface_, SIGNAL(orientationChanged(const Unsigned&)), this, SLOT(receivedData(const Unsigned&)));
    }
    else if (sensorName_ == "accelerometersensor")
    {
        sensorChannelInterface_ = AccelerometerSensorChannelInterface::interface("accelerometersensor");
        connect(sensorChannelInterface_, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(receivedData(const XYZ&)));
        connect(sensorChannelInterface_, SIGNAL(frameAvailable(const QVector<XYZ>&)), this, SLOT(receivedFrame(const QVector<XYZ>&)));
    }
    else if (sensorName_ == "alssensor")
    {
        sensorChannelInterface_ = ALSSensorChannelInterface::interface("alssensor");
        connect(sensorChannelInterface_, SIGNAL(ALSChanged(const Unsigned&)), this, SLOT(receivedData(const Unsigned&)));
    }
    else if (sensorName_ == "rotationsensor")
    {
        sensorChannelInterface_ = RotationSensorChannelInterface::interface("rotationsensor");
        connect(sensorChannelInterface_, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(receivedData(const XYZ&)));
        connect(sensorChannelInterface_, SIGNAL(frameAvailable(const QVector<XYZ>&)), this, SLOT(receivedFrame(const QVector<XYZ>&)));
    }
    else if (sensorName_ == "tapsensor")
    {
        sensorChannelInterface_ = TapSensorChannelInterface::interface("tapsensor");
        connect(sensorChannelInterface_, SIGNAL(dataAvailable(const Tap&)), this, SLOT(receivedData(const Tap&)));
    }
    else if (sensorName_ == "proximitysensor")
    {
        sensorChannelInterface_ = ProximitySensorChannelInterface::interface("proximitysensor");
        connect(sensorChannelInterface_, SIGNAL(dataAvailable(const Unsigned&)), this, SLOT(receivedData(const Unsigned&)));
    }

}
