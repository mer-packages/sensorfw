/**
   @file sensorhandler.h
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
#include <QThread>

#include "qt-api/sensormanagerinterface.h"
#include "qt-api/orientationsensor_i.h"
#include "qt-api/accelerometersensor_i.h"
#include "qt-api/compasssensor_i.h"
#include "qt-api/tapsensor_i.h"
#include "qt-api/alssensor_i.h"
#include "qt-api/proximitysensor_i.h"
#include "qt-api/rotationsensor_i.h"
#include "qt-api/magnetometersensor_i.h"

class SensorHandler : public QThread
{
    Q_OBJECT
public:

    SensorHandler(const QString& sensorName, QObject *parent = 0);

    void startClient();
    void createSensorInterface();
    void run();

    int dataCount() const { return dataCount_; }
    QString sensorName() const { return sensorName_; }

public Q_SLOTS:
    void receivedData(const MagneticField& data);
    void receivedData(const XYZ& data);
    void receivedData(const Compass& value);
    void receivedData(const Unsigned& data);
    void receivedData(const Tap& data);
    void receivedFrame(const QVector<MagneticField>& frame);
    void receivedFrame(const QVector<XYZ>& frame);

private:
    QString sensorName_;
    AbstractSensorChannelInterface* sensorChannelInterface_;
    int interval_;
    int bufferinterval_;
    bool standbyoverride_;
    int buffersize_;
    int dataCount_;
    int frameCount_;
};

#endif // SENSORHANDLER_H
