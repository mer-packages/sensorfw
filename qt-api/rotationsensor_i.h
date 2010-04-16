/**
   @file rotationsensor_i.h
   @brief Interface for RotationSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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

#ifndef ROTATIONSENSOR_I_H
#define ROTATIONSENSOR_I_H

#include <QtDBus/QtDBus>

#include "abstractsensor_i.h"
#include <datatypes/xyz.h>

/**
 * @brief DBus-interface for listening on device rotation changes.
 * 
 * Acts as a proxy class for interface \e local.RotationSensor interface.
 *
 * For details of measurement process, see #RotationSensorChannel. 
 */
class RotationSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT;
    Q_PROPERTY(XYZ rotation READ rotation);
    Q_PROPERTY(bool hasZ READ hasZ);

public:
    static inline const char *staticInterfaceName()
    { return "local.RotationSensor"; }

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId)
    {
        // ToDo: see which arguments can be made explicit
        return new RotationSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
    }

    inline XYZ rotation() const { return qvariant_cast< XYZ >(internalPropGet("rotation")); }
    inline bool hasZ() const { return qvariant_cast< bool >(internalPropGet("hasZ")); }

public:
    RotationSensorChannelInterface(const QString &path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static const RotationSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static RotationSensorChannelInterface* controlInterface(const QString& id);

public Q_SLOTS: // METHODS
    void dataReceived();

Q_SIGNALS: // SIGNALS
    /**
     * Sent when device rotation has changed.
     * @param data Current device rotation.
     */
    void dataAvailable(const XYZ& data);
};

namespace local {
  typedef ::RotationSensorChannelInterface RotationSensor;
}

#endif /* ROTATIONSENSOR_I_H */
