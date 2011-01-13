/**
   @file sensormanager_a.h
   @brief D-Bus adaptor for SensorManager

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#ifndef SENSORMANAGER_A_H
#define SENSORMANAGER_A_H

#include <QtDBus/QtDBus>
#include "sensormanager.h"

/*
 * Adaptor class for interface local.SensorManager
 */
class SensorManagerAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.SensorManager")
    Q_PROPERTY(QString errorString READ errorString)
    Q_PROPERTY(int errorCodeInt READ errorCodeInt)

public:
    SensorManagerAdaptor(QObject *parent);
    virtual ~SensorManagerAdaptor() {}

    QString errorString() const;

private: // PROPERTIES
    int errorCodeInt() const;
    SensorManager* sensorManager() const;

public Q_SLOTS: // METHODS
    bool loadPlugin(const QString& name);

    int  requestSensor(const QString &id, qint64 pid);
    bool releaseSensor(const QString &id, int sessionId, qint64 pid);

Q_SIGNALS: // SIGNALS
    void errorSignal(int error);
};

#endif
