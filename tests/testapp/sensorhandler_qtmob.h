/**
   @file sensorhandler_qmsystem2.h
   @brief test application to create sensor

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

#ifndef SENSORHANDLER_H
#define SENSORHANDLER_H

#include <QVector>
#include "config.h"
#include "abstractsensorhandler.h"

#include <QSensor>
#include <QAccelerometer>
#include <QCompass>
#include <QMagnetometer>
#include <QOrientationSensor>
#include <QAmbientLightSensor>
#include <QRotationSensor>
#include <QTapSensor>
#include <QProximitySensor>

#include <QAccelerometerReading>
#include <QCompassReading>
#include <QMagnetometerReading>
#include <QOrientationReading>
#include <QAmbientLightReading>
#include <QRotationReading>
#include <QTapReading>
#include <QProximityReading>


QTM_USE_NAMESPACE

class SensorHandler : public AbstractSensorHandler
{
    Q_OBJECT

public:

    SensorHandler(const QString& sensorName, QObject *parent = 0);
    ~SensorHandler();
    virtual bool startClient();
    virtual bool stopClient();

    int dataCount() const { return dataCount_; }
    QString sensorName() const { return sensorName_; }

    static void init(const QStringList& sensors);

public Q_SLOTS:
    void receivedData(const QAccelerometerReading* data);
    void receivedData(const QAmbientLightReading* data);
    void receivedData(const QCompassReading* data);
    void receivedData(const QMagnetometerReading* data);
    void receivedData(const QOrientationReading* data);
    void receivedData(const QRotationReading* data);
    void receivedData(const QTapReading* data);
    void receivedData(const QProximityReading* data);

private:
    QSensor* m_sensor;
};

#endif // SENSORHANDLER_H
