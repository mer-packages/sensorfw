/**
   @file sensormanager_i.cpp
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

#include "sensormanager_i.h"

/*
 * Implementation of interface class LocalSensorManagerInterface
 */

const char* LocalSensorManagerInterface::staticInterfaceName = "local.SensorManager";

LocalSensorManagerInterface::LocalSensorManagerInterface(const QString& service, const QString& path, const QDBusConnection& connection, QObject* parent) :
    QDBusAbstractInterface(service, path, staticInterfaceName, connection, parent)
{
}

LocalSensorManagerInterface::~LocalSensorManagerInterface()
{
}

SensorManagerError LocalSensorManagerInterface::errorCode() const
{
    return static_cast<SensorManagerError>(errorCodeInt());
}

QString LocalSensorManagerInterface::errorString() const
{
    return qvariant_cast<QString>(internalPropGet("errorString"));
}

int LocalSensorManagerInterface::errorCodeInt() const
{
    return static_cast<SensorManagerError>(qvariant_cast<int>(internalPropGet("errorCodeInt")));
}

QDBusReply<bool> LocalSensorManagerInterface::loadPlugin(const QString& name)
{
    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(name);
    return callWithArgumentList(QDBus::Block, QLatin1String("loadPlugin"), argumentList);
}

QDBusReply<int> LocalSensorManagerInterface::requestSensor(const QString& id)
{
    qint64 pid = QCoreApplication::applicationPid();
    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(id);
    argumentList << qVariantFromValue(pid);
    return callWithArgumentList(QDBus::Block, QLatin1String("requestSensor"), argumentList);
}

QDBusReply<bool> LocalSensorManagerInterface::releaseSensor(const QString& id, int sessionId)
{
    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(id) << qVariantFromValue(sessionId);
    qint64 pid = QCoreApplication::applicationPid();
    argumentList << qVariantFromValue(pid);
    return callWithArgumentList(QDBus::Block, QLatin1String("releaseSensor"), argumentList);
}
