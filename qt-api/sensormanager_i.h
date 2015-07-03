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

/**
 * DBus interface to SensorManager instance.
 */
class LocalSensorManagerInterface : public QDBusAbstractInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(LocalSensorManagerInterface)
    Q_PROPERTY(SensorManagerError errorCode READ errorCode)
    Q_PROPERTY(QString errorString READ errorString)
    Q_PROPERTY(int errorCodeInt READ errorCodeInt NOTIFY errorSignal)

    /**
     * Get error code integer.
     *
     * @return error code interger.
     */
    int errorCodeInt();

public:
    /**
     * Get name of the D-Bus interface for this class.
     *
     * @return Name of the interface.
     */
    static const char* staticInterfaceName;

    /**
     * Destructor.
     */
    virtual ~LocalSensorManagerInterface();

    /**
     * Get error code of occured remote error.
     *
     * @return error code.
     */
    SensorManagerError errorCode();

    /**
     * Get error description of occured remote error.
     *
     * @return error description.
     */
    QString errorString();

public Q_SLOTS:

    /**
     * Request sensor daemon to load plugin.
     *
     * @param name plugin name.
     * @return DBus reply.
     */
    QDBusReply<bool> loadPlugin(const QString& name);

    /**
     * Request sensor deamon to start new session for sensor.
     *
     * @param id sensor ID.
     * @return DBus reply.
     */
    QDBusReply<int> requestSensor(const QString& id);

    /**
     * Request sensor deamon to release existing session.
     *
     * @param id sensor ID.
     * @param sessionId session ID.
     * @return DBus reply.
     */
    QDBusReply<bool> releaseSensor(const QString& id, int sessionId);

Q_SIGNALS:

    /**
     * Signal about occured errors.
     *
     * @param error Occured error code.
     */
    void errorSignal(int error);

    void loadPluginFinished();
    void requestSensorFinished();
    void releaseSensorFinished();

protected:

    /**
     * Constructor.
     *
     * @param service DBus service name.
     * @param path DBus service path.
     * @param connection DBUs connection.
     * @param parent Parent object.
     */
    LocalSensorManagerInterface(const QString& service, const QString& path, const QDBusConnection& connection, QObject* parent = 0);

protected slots:
    void loadPluginFinished(QDBusPendingCallWatcher *watch);
    void requestSensorFinished(QDBusPendingCallWatcher *watch);
    void releaseSensorFinished(QDBusPendingCallWatcher *watch);

};

namespace local {
  typedef ::LocalSensorManagerInterface SensorManager;
}
#endif
