/**
   @file alssensor_i.h
   @brief Interface for ALSSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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
    Q_DISABLE_COPY(ALSSensorChannelInterface)
    Q_PROPERTY(Unsigned lux READ lux)

public:
    static const char* staticInterfaceName;

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId);

    Unsigned lux();

public:
    ALSSensorChannelInterface(const QString& path, int sessionId);

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

    /**
     * Request a interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static ALSSensorChannelInterface* interface(const QString& id);

protected:
    virtual bool dataReceivedImpl();

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
