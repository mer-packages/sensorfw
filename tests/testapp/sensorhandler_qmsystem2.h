/**
   @file sensorhandler_qmsystem2.h
   @brief test application to create sensor

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

#ifndef SENSORHANDLER_H
#define SENSORHANDLER_H

#include <QVector>
#include <QThread>
#include "abstractsensorhandler.h"
#include "qmsensor.h"
#include "qmtap.h"
#include "qmals.h"
#include "qmaccelerometer.h"
#include "qmmagnetometer.h"
#include "qmcompass.h"
#include "qmproximity.h"
#include "qmorientation.h"
#include "qmrotation.h"
#include "config.h"

class SensorHandler : public AbstractSensorHandler
{
    Q_OBJECT
public:

    SensorHandler(const QString& sensorName, QObject *parent = 0);

    static bool init(const QStringList& sensors);

    virtual bool startClient();
    virtual bool stopClient();

public Q_SLOTS:
    void receivedData(const MeeGo::QmAccelerometerReading& data);
    void receivedData(const MeeGo::QmIntReading data);
    void receivedData(const MeeGo::QmCompassReading data);
    void receivedData(const MeeGo::QmMagnetometerReading& data);
    void receivedData(const MeeGo::QmOrientationReading data);
    void receivedData(const MeeGo::QmRotationReading& data);
    void receivedData(const MeeGo::QmTapReading data);

private:
    MeeGo::QmSensor* sensor_;
};

#endif // SENSORHANDLER_H
