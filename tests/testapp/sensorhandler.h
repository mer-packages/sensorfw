/**
   @file testthread.h
   @brief test application to create sensor

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

#ifndef SENSORHANDLER_H
#define SENSORHANDLER_H

#include <QVector>
#include "qt-api/abstractsensor_i.h"
#include "datatypes/xyz.h"
#include "config.h"

#include "qt-api/sensormanagerinterface.h"
#include "qt-api/orientationsensor_i.h"
#include "qt-api/accelerometersensor_i.h"
#include "qt-api/compasssensor_i.h"
#include "qt-api/tapsensor_i.h"
#include "qt-api/alssensor_i.h"
#include "qt-api/proximitysensor_i.h"
#include "qt-api/rotationsensor_i.h"
#include "qt-api/magnetometersensor_i.h"


class SensorHandler : QObject
{
    Q_OBJECT
public:

    SensorHandler(const QString& sensorName, QObject *parent = 0);

    void setInterval(int value);
    void setBufferInterval(int value);
    void setBufferSize(int value);
    void setStandbyOverride(bool value);

    void start();

public Q_SLOTS:
    void receivedData(const MagneticField& data);
    void receivedData(const XYZ& data);
    void receivedData(const Compass& value);
    void receivedData(const Unsigned& data);
    void receivedData(const Tap& data);
    void receivedFrame(const QVector<MagneticField>& frame);
    void receivedFrame(const QVector<XYZ>& frame);

private:
    QString sensorName;
    AbstractSensorChannelInterface* sensorChannelInterface;
    int interval;
    int bufferinterval;
    bool standbyoverride;
    int buffersize;
    int dataCount;
    int frameCount;
};

#endif // SENSORHANDLER_H
