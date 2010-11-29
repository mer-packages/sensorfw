/**
   @file sensormanager_i.h
   @brief Proxy class for interface for SensorManager

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Semi Malinen <semi.malinen@nokia.com
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#ifndef SENSORMANAGER_I_H
#define SENSORMANAGER_I_H

#include <QtDBus/QtDBus>
#include <QString>
#include "sfwerror.h"

/*
 * Proxy class for interface local.SensorManager
 */
class LocalSensorManagerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(LocalSensorManagerInterface)
    Q_PROPERTY(SensorManagerError errorCode READ errorCode)
    Q_PROPERTY(QString errorString READ errorString)
    Q_PROPERTY(int errorCodeInt READ errorCodeInt)

    int errorCodeInt() const; // this exists as a hack because enums cannot be marshalled over D-BUS

public:
    static const char* staticInterfaceName;

    virtual ~LocalSensorManagerInterface();

    SensorManagerError errorCode() const;
    QString errorString() const;

public Q_SLOTS: // METHODS
    QDBusReply<bool> loadPlugin(const QString& name);

    QDBusReply<int> requestControlSensor(const QString& id);

    QDBusReply<int> requestListenSensor(const QString& id);

    QDBusReply<bool> releaseSensor(const QString& id, int sessionId);

Q_SIGNALS: // SIGNALS
    void errorSignal(int error);

protected:
    LocalSensorManagerInterface(const QString& service, const QString& path, const QDBusConnection& connection, QObject* parent = 0);
};

namespace local {
  typedef ::LocalSensorManagerInterface SensorManager;
}
#endif
