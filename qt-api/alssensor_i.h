/**
   @file alssensor_i.h
   @brief Interface for ALSSensor

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

#ifndef ALSSENSOR_I_H
#define ALSSENSOR_I_H

#include <QtDBus/QtDBus>

#include "datatypes/unsigned.h"
#include "abstractsensor_i.h"

/**
 * @brief DBus-interface for accessing ambient light sensor measurements.
 *
 * ALSSensorChannelInterface acts as proxy class for interface \c local.ALSSensor.
 * Provides signal on change of measured ambient light intensity level. Previous
 * measured intensity level can be queried any time. Provided values are in \e lux.
 *
 * For details of measurement process, see #ALSSensorChannel.
 *
 * @todo Add property descriptions.
 */
class ALSSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT

public:
    static inline const char *staticInterfaceName()
    { return "local.ALSSensor"; }

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId)
    {
        // ToDo: see which arguments can be made explicit
        return new ALSSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
    }

    Q_PROPERTY(Unsigned lux READ lux)
    inline Unsigned lux() const { return qvariant_cast< Unsigned >(internalPropGet("lux")); }

public:
    ALSSensorChannelInterface(const QString &path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static const ALSSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static ALSSensorChannelInterface* controlInterface(const QString& id);

public Q_SLOTS: // METHODS
    void dataReceived();

Q_SIGNALS: // SIGNALS
    /**
     * Sent when measured ambient light intensity has changed.
     * @param value Observed ambient light intensity.
     */
    void ALSChanged(const Unsigned& value);

};

namespace local {
  typedef ::ALSSensorChannelInterface ALSSensor;
}

#endif
