/**
   @file testthread.cpp
   @brief test application to create sensor
   <p>
   Copyright (C) 2010-2011 Nokia Corporation

   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>
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

#include <QTest>
#include <QVector>
#include <QDebug>

#include "qt-api/sensormanagerinterface.h"
#include "qt-api/orientationsensor_i.h"
#include "qt-api/accelerometersensor_i.h"
#include "qt-api/compasssensor_i.h"
#include "qt-api/tapsensor_i.h"
#include "qt-api/alssensor_i.h"
#include "qt-api/proximitysensor_i.h"
#include "qt-api/rotationsensor_i.h"
#include "qt-api/magnetometersensor_i.h"

#include "testthread.h"


void Testthread::setInterface(AbstractSensorChannelInterface* inf)
{
    sensorChannelInterface = inf;
}


Testthread::Testthread(QObject *parent) :
        QThread(parent)
{

}

Testthread::Testthread(QString sensorName, QObject *parent) :
    QThread(parent), sensorName(sensorName), interval(10), bufferinterval(0), dataCount(0), frameCount(0) 
{
    if (sensorName == "compasssensor")
    {
        sensorChannelInterface = CompassSensorChannelInterface::interface("compasssensor");
    } else if (sensorName == "magnetometersensor")
    {
        sensorChannelInterface = MagnetometerSensorChannelInterface::interface("magnetometersensor");
    } else if (sensorName ==  "orientationsensor")
    {
        sensorChannelInterface = OrientationSensorChannelInterface::interface("orientationsensor");
    } else if (sensorName == "accelerometersensor")
    {
        sensorChannelInterface = AccelerometerSensorChannelInterface::interface("accelerometersensor");
    } else if (sensorName == "alssensor")
    {
        sensorChannelInterface = ALSSensorChannelInterface::interface("alssensor");
    } else if (sensorName == "rotationsensor")
    {
        sensorChannelInterface = RotationSensorChannelInterface::interface("rotationsensor");
    } else if (sensorName == "tapsensor")
    {
        sensorChannelInterface = TapSensorChannelInterface::interface("tapsensor");
    } else if (sensorName == "proximitysensor")
    {
        sensorChannelInterface = ProximitySensorChannelInterface::interface("proximitysensor");
    }
}


void Testthread::setInterval(int value)
{
    interval = value;
}

void Testthread::setBufferInterval(int value)
{
    bufferinterval = value;
}

void Testthread::receivedData(XYZ& data)
{
    dataCount ++;
    qDebug() << this->objectName() <<" received the " << dataCount << "data :" 
             << data.x() << " " <<   data.y() << " " <<   data.z();
}

void Testthread::receivedFrame(QVector<XYZ>& frame)
{
    frameCount ++;
    qDebug() << this->objectName() << " received the " << frameCount << "frame :"; 
    for (int i = 0; i < frame.size(); i++)
        qDebug() << frame.at(i).x() << " " << frame.at(i).y() << " " << frame.at(i).z();
}

void Testthread::run()
{
    sensorChannelInterface->setInterval(interval);
    sensorChannelInterface->setBufferInterval(bufferinterval);
    sensorChannelInterface->setStandbyOverride(true);
    connect(sensorChannelInterface, SIGNAL(dataAvailable(XYZ&)), this, SLOT(receivedData(XYZ&)));
    connect(sensorChannelInterface, SIGNAL(frameAvailable(QVector<XYZ>&)), this, SLOT(receivedFrame(QVector<XYZ>&)));
    sensorChannelInterface->start();
    exec();
}
