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

class SensorHandler : public QThread
{
    Q_OBJECT
public:

    SensorHandler(const QString& sensorName, QObject *parent = 0);

    void startClient();

    int dataCount() const { return dataCount_; }
    QString sensorName() const { return sensorName_; }

    static void init(const QStringList& sensors);

public Q_SLOTS:
    void receivedData(const MeeGo::QmAccelerometerReading& data);
    void receivedALSData(const MeeGo::QmAlsReading data);
    void receivedData(const MeeGo::QmCompassReading data);
    void receivedData(const MeeGo::QmMagnetometerReading& data);
    void receivedData(const MeeGo::QmOrientationReading data);
    void receivedData(const MeeGo::QmProximityReading data);
    void receivedData(const MeeGo::QmRotationReading& data);
    void receivedData(const MeeGo::QmTapReading data);

private:
    MeeGo::QmSensor* sensor_;
    QString sensorName_;
    int interval_;
    int bufferinterval_;
    bool standbyoverride_;
    int buffersize_;
    int dataCount_;
    int frameCount_;
};

#endif // SENSORHANDLER_H
