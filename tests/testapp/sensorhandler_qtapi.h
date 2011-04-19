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
#include "abstractsensor_i.h"
#include "datatypes/xyz.h"
#include "config.h"
#include "abstractsensorhandler.h"

#include "sensormanagerinterface.h"
#include "orientationsensor_i.h"
#include "accelerometersensor_i.h"
#include "compasssensor_i.h"
#include "tapsensor_i.h"
#include "alssensor_i.h"
#include "proximitysensor_i.h"
#include "rotationsensor_i.h"
#include "magnetometersensor_i.h"

class SensorHandler : public AbstractSensorHandler
{
    Q_OBJECT
public:
    SensorHandler(const QString& sensorName, QObject *parent = 0);

    static bool init(const QStringList& sensors);

    virtual bool startClient();
    virtual bool stopClient();

public Q_SLOTS:
    void receivedData(const MagneticField& data);
    void receivedData(const XYZ& data);
    void receivedData(const Compass& value);
    void receivedData(const Unsigned& data);
    void receivedData(const Tap& data);
    void receivedFrame(const QVector<MagneticField>& frame);
    void receivedFrame(const QVector<XYZ>& frame);
    void receivedData(const Proximity& data);

private:
    void createSensorInterface();

    static QString toString(const DataRangeList& ranges);

    AbstractSensorChannelInterface* sensorChannelInterface_;
};

#endif // SENSORHANDLER_H
