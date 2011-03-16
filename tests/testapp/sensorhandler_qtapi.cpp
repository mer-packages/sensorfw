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

#include "sensorhandler_qtapi.h"
#include "logging.h"
#include "qt-api/sensormanagerinterface.h"
#include "qt-api/orientationsensor_i.h"
#include "qt-api/accelerometersensor_i.h"
#include "qt-api/compasssensor_i.h"
#include "qt-api/tapsensor_i.h"
#include "qt-api/alssensor_i.h"
#include "qt-api/proximitysensor_i.h"
#include "qt-api/rotationsensor_i.h"
#include "qt-api/magnetometersensor_i.h"

SensorHandler::SensorHandler(const QString& sensorName, QObject *parent) :
    AbstractSensorHandler(sensorName, parent),
    sensorChannelInterface_(NULL)
{
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

void SensorHandler::receivedData(const Proximity& data)
{
    dataCount_++;
    sensordLogT() << this->objectName() << " sample " << dataCount_ << ": "
                  << data.UnsignedData().value_ << " " << data.proximityData().value_;
}

void SensorHandler::receivedFrame(const QVector<MagneticField>& frame)
{
    frameCount_++;
    sensordLogT() << this->objectName() << " frame " << frameCount_ << " size " << frame.size();
    foreach (const MagneticField& data, frame)
    {
        sensordLogT() << data.x() << " " << data.y() << " " << data.z();
    }
}

void SensorHandler::receivedFrame(const QVector<XYZ>& frame)
{
    frameCount_++;
    sensordLogT() << this->objectName() << " frame " << frameCount_ << " size " << frame.size();
    foreach (const XYZ& data, frame)
    {
        sensordLogT() << data.x() << " " << data.y() << " " << data.z();
    }
}

bool SensorHandler::startClient()
{
    createSensorInterface();
    if (sensorChannelInterface_ == 0)
    {
         sensordLogD() << "Creating sensor client interface fails.";
         return false;
    }
    sensordLogD() << "Created sensor: " << sensorChannelInterface_->description();
    sensordLogD() << "Support intervals: " << toString(sensorChannelInterface_->getAvailableIntervals());
    sensordLogD() << "Support dataranges: " << toString(sensorChannelInterface_->getAvailableDataRanges());
    sensorChannelInterface_->setInterval(interval_);
    sensorChannelInterface_->setBufferInterval(bufferinterval_);
    sensorChannelInterface_->setBufferSize(buffersize_);
    sensorChannelInterface_->setStandbyOverride(standbyoverride_);
    sensorChannelInterface_->setDownsampling(downsample_);
    sensorChannelInterface_->start();

    return true;
}

bool SensorHandler::stopClient()
{
    if (sensorChannelInterface_)
    {
        sensorChannelInterface_->stop();
        delete sensorChannelInterface_;
        sensorChannelInterface_ = 0;
    }
    return true;
}

bool SensorHandler::init(const QStringList& sensors)
{
    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
    if(!remoteSensorManager.isValid())
    {
        sensordLogC() << "Failed to create SensorManagerInterface";
        return false;
    }
    foreach (const QString& sensorName, sensors)
    {
        QDBusReply<bool> reply(remoteSensorManager.loadPlugin(sensorName));
        if(!reply.isValid() || !reply.value())
        {
            sensordLogW() << "Failed to load plugin";
            return false;
        }

        if (sensorName == "orientationsensor"){
            remoteSensorManager.registerSensorInterface<OrientationSensorChannelInterface>(sensorName);
        } else if (sensorName == "accelerometersensor"){
            remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>(sensorName);
        } else if (sensorName == "compasssensor"){
            remoteSensorManager.registerSensorInterface<CompassSensorChannelInterface>(sensorName);
        } else if (sensorName == "tapsensor"){
            remoteSensorManager.registerSensorInterface<TapSensorChannelInterface>(sensorName);
        } else if (sensorName == "alssensor"){
            remoteSensorManager.registerSensorInterface<ALSSensorChannelInterface>(sensorName);
        } else if (sensorName == "proximitysensor"){
            remoteSensorManager.registerSensorInterface<ProximitySensorChannelInterface>(sensorName);
        } else if (sensorName == "rotationsensor"){
            remoteSensorManager.registerSensorInterface<RotationSensorChannelInterface>(sensorName);
        } else if (sensorName == "magnetometersensor"){
            remoteSensorManager.registerSensorInterface<MagnetometerSensorChannelInterface>(sensorName);
        }
    }
    return true;
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
        connect(sensorChannelInterface_, SIGNAL(reflectanceDataAvailable(const Proximity&)), this, SLOT(receivedData(const Proximity&)));
    }
}

QString SensorHandler::toString(const DataRangeList& ranges)
{
    QString str;
    bool first = true;
    foreach(const DataRange& range, ranges)
    {
        if(!first)
            str.append(", ");
        str.append("[")
            .append(QVariant(range.min).toString())
            .append(", ")
            .append(QVariant(range.max).toString())
            .append("]");
        first = false;
    }
    return str;
}
