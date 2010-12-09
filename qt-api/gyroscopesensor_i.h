/**
   @file gyroscopesensor_i.h
   @brief Interface for GyroscopeSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>

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

#ifndef GYROSCOPESENSOR_I_H
#define GYROSCOPESENSOR_I_H

#include <QtDBus/QtDBus>

#include "abstractsensor_i.h"
#include <datatypes/xyz.h>

/**
 * @brief DBus-interface for accessing raw gyroscope values.
 *
 * GyroscopeSensorChannelInterface acts as a proxy class for interface
 * \e local.GyroscopeSensor.
 *
 * For details of measurement process, see #GyroscopeSensorChannel.
 */
class GyroscopeSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT;

public:
    /**
     * Get name of the D-Bus interface for this class.
     * @return Name of the interface.
     */
    static inline const char *staticInterfaceName()
    { return "local.GyroscopeSensor"; }

    /**
     * Get an instance of the class.
     * @return Pointer to new instance of the class.
     */
    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId)
    {
        // ToDo: see which arguments can be made explicit
        return new GyroscopeSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
    }

    Q_PROPERTY(XYZ value READ get);
    inline XYZ get() const { return qvariant_cast<XYZ>(internalPropGet("value")); }

public:
    /**
     * Constructor.
     * @param path      path.
     * @param sessionid session id.
     */
    GyroscopeSensorChannelInterface(const QString &path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static const GyroscopeSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static GyroscopeSensorChannelInterface* controlInterface(const QString& id);

protected:
    virtual void connectNotify(const char* signal);


private:
    bool frameAvailableConnected;


public Q_SLOTS: // METHODS
    void dataReceived();
    QDBusReply<void> reset();


Q_SIGNALS: // SIGNALS
    /**
     * Sent when new measurement data has become available.
     * @param data New measurement data.
     */
    void dataAvailable(const XYZ& data);


    /**
     * Sent when new measurement frame has become available.
     * If app doesn't connect to this signal content of frames
     * will be sent through dataAvailable signal.
     * @param data New measurement frame.
     */
    void frameAvailable(const QVector<XYZ>& frame);

};

namespace local {
  typedef ::GyroscopeSensorChannelInterface GyroscopeSensor;
}

#endif
