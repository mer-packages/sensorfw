/**
   @file sensormanager_a.cpp
   @brief D-Bus adaptor for SensorManager

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Semi Malinen <semi.malinen@nokia.com

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

#include "sensormanager_a.h"
#include "logging.h"

#include "sfwerror.h"

/*
 * Implementation of adaptor class SensorManagerAdaptor
 */

SensorManagerAdaptor::SensorManagerAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

int SensorManagerAdaptor::errorCodeInt() const
{
    // note: enums can not be transferred over D-BUS
    return static_cast<SensorManagerError>(qvariant_cast< int >(parent()->property("errorCodeInt")));
}

QString SensorManagerAdaptor::errorString() const
{
    return qvariant_cast< QString >(parent()->property("errorString"));
}

bool SensorManagerAdaptor::loadPlugin(const QString& name)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "loadPlugin", Q_RETURN_ARG(bool, out0), Q_ARG(QString, name));
    return out0;
}

int SensorManagerAdaptor::requestControlSensor(const QString &id, qint64 pid)
{
    QString dbg("Control sensor requested. Client PID: ");
    dbg += QString::number(pid);
    sensordLog() << dbg;
    // handle method call local.SensorManager.requestControlSensor
    int out0;
    QMetaObject::invokeMethod(parent(), "requestControlSensor", Q_RETURN_ARG(int, out0), Q_ARG(QString, id));
    return out0;
}

int SensorManagerAdaptor::requestListenSensor(const QString &id, qint64 pid)
{
    QString dbg("Listen sensor requested. Client PID: ");
    dbg += QString::number(pid);
    sensordLog() << dbg;
    // handle method call local.SensorManager.requestListenSensor
    int out0;
    QMetaObject::invokeMethod(parent(), "requestListenSensor", Q_RETURN_ARG(int, out0), Q_ARG(QString, id));
    return out0;
}

bool SensorManagerAdaptor::releaseSensor(const QString &id, int sessionId, qint64 pid)
{
    QString dbg("Sensor release requested. Client PID: ");
    dbg += QString::number(pid);
    sensordLog() << dbg;
    // handle method call local.SensorManager.releaseListenSensor
    bool out0;
    QMetaObject::invokeMethod(parent(), "releaseSensor", Q_RETURN_ARG(bool, out0), Q_ARG(QString, id), Q_ARG(int, sessionId));
    return out0;
}
