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

#include <QTest>
#include <QVector>

#include "sensorhandler.h"
#include "logging.h"

SensorHandler::SensorHandler(const QString& sensorName, QObject *parent) :
    QThread(parent),
    sensorName(sensorName),
    sensorChannelInterface(NULL),
    interval(100),
    bufferinterval(0),
    standbyoverride(false),
    buffersize(0),
    dataCount(0),
    frameCount(0)
{
    if (sensorName == "compasssensor")
    {
        sensorChannelInterface = CompassSensorChannelInterface::interface("compasssensor");
        connect(sensorChannelInterface, SIGNAL(dataAvailable(const Compass&)), this, SLOT(receivedData(const Compass&)));
    }
    else if (sensorName == "magnetometersensor")
    {
        sensorChannelInterface = MagnetometerSensorChannelInterface::interface("magnetometersensor");
        connect(sensorChannelInterface, SIGNAL(dataAvailable(const MagneticField&)), this, SLOT(receivedData(const MagneticField&)));
        connect(sensorChannelInterface, SIGNAL(frameAvailable(const QVector<MagneticField>&)), this, SLOT(receivedFrame(const QVector<MagneticField>&)));

    }
    else if (sensorName ==  "orientationsensor")
    {
        sensorChannelInterface = OrientationSensorChannelInterface::interface("orientationsensor");
        connect(sensorChannelInterface, SIGNAL(dataAvailable(const Unsigned&)), this, SLOT(receivedData(const Unsigned&)));
    }
    else if (sensorName == "accelerometersensor")
    {
        sensorChannelInterface = AccelerometerSensorChannelInterface::interface("accelerometersensor");
        connect(sensorChannelInterface, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(receivedData(const XYZ&)));
        connect(sensorChannelInterface, SIGNAL(frameAvailable(const QVector<XYZ>&)), this, SLOT(receivedFrame(QVector<const XYZ>&)));
    }
    else if (sensorName == "alssensor")
    {
        sensorChannelInterface = ALSSensorChannelInterface::interface("alssensor");
        connect(sensorChannelInterface, SIGNAL(dataAvailable(const Unsigned&)), this, SLOT(receivedData(const Unsigned&)));
    }
    else if (sensorName == "rotationsensor")
    {
        sensorChannelInterface = RotationSensorChannelInterface::interface("rotationsensor");
        connect(sensorChannelInterface, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(receivedData(const XYZ&)));
        connect(sensorChannelInterface, SIGNAL(frameAvailable(const QVector<XYZ>&)), this, SLOT(receivedFrame(const QVector<XYZ>&)));
    }
    else if (sensorName == "tapsensor")
    {
        sensorChannelInterface = TapSensorChannelInterface::interface("tapsensor");
        connect(sensorChannelInterface, SIGNAL(dataAvailable(const Tap&)), this, SLOT(receivedData(const Tap&)));
    }
    else if (sensorName == "proximitysensor")
    {
        sensorChannelInterface = ProximitySensorChannelInterface::interface("proximitysensor");
        connect(sensorChannelInterface, SIGNAL(dataAvailable(const Unsigned&)), this, SLOT(receivedData(const Unsigned&)));
    }

    if (Config::configuration() != NULL)
    {
        interval = Config::configuration()->value(sensorName+"/interval", "100").toInt();
        bufferinterval = Config::configuration()->value(sensorName+"/bufferinterval", "0").toInt();
        standbyoverride = Config::configuration()->value(sensorName+"/standbyoverride", "false").toBool();
        buffersize = Config::configuration()->value(sensorName+"/buffersize", "0").toInt();

    }
}

void SensorHandler::setInterval(int value)
{
    interval = value;
}

void SensorHandler::setBufferInterval(int value)
{
    bufferinterval = value;
}

void SensorHandler::receivedData(const MagneticField& data)
{
    dataCount ++;
    sensordLogT() << this->objectName() << " sample " << dataCount << ": "
                  << data.x() << " " << data.y() << " " <<   data.z();
}

void SensorHandler::receivedData(const XYZ& data)
{
    dataCount ++;
    sensordLogT() << this->objectName() << " sample " << dataCount << ": "
                  << data.x() << " " << data.y() << " " <<   data.z();
}

void SensorHandler::receivedData(const Compass& data)
{
    dataCount ++;
    sensordLogT() << this->objectName() << " sample " << dataCount << ": "
                  << data.degrees() << " " << data.level();
}

void SensorHandler::receivedData(const Unsigned& data)
{
    dataCount ++;
    sensordLogT() << this->objectName() << " sample " << dataCount << ": "
                  << data.x();
}

void SensorHandler::receivedData(const Tap& data)
{
    dataCount ++;
    sensordLogT() << this->objectName() << " sample " << dataCount << ": "
                  << data.direction() << " " << data.type();
}

void SensorHandler::receivedFrame(const QVector<MagneticField>& frame)
{
    frameCount ++;
    sensordLogT() << this->objectName() << " frame " << frameCount << " size " << frame.size();
    foreach (const MagneticField& data, frame)
        sensordLogT() << data.x() << " " << data.y() << " " << data.z();
}

void SensorHandler::receivedFrame(const QVector<XYZ>& frame)
{
    frameCount ++;
    sensordLogT() << this->objectName() << " frame " << frameCount << " size " << frame.size();
    foreach (const XYZ& data, frame)
        sensordLogT() << data.x() << " " << data.y() << " " << data.z();
}


void SensorHandler::startClient()
{
    sensorChannelInterface->setInterval(interval);
    sensorChannelInterface->setBufferInterval(bufferinterval);
    sensorChannelInterface->setBufferSize(buffersize);
    sensorChannelInterface->setStandbyOverride(standbyoverride);
    sensorChannelInterface->start();
}
