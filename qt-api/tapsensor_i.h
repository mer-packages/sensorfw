/**
   @file tapsensor_i.h
   @brief Interface for TapSensor

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

#ifndef TAPSENSOR_I_H
#define TAPSENSOR_I_H

#include <QtDBus/QtDBus>

#include "abstractsensor_i.h"
#include <datatypes/tap.h>

/**
 * @brief DBus-interface for accessing device tap events.
 *
 * Acts as a proxy class for interface \e local.TapSensor.
 *
 * For details of measurement process, see #TapSensorChannel.
 * @todo Add member and method descriptions.
 */
class TapSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT;

public:
    static inline const char *staticInterfaceName()
    { return "local.TapSensor"; }

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId)
    {
        // ToDo: see which arguments can be made explicit
        return new TapSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
    }

public:
    TapSensorChannelInterface(const QString &path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static const TapSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static TapSensorChannelInterface* controlInterface(const QString& id);

private Q_SLOTS: // METHODS
    void dataReceived();

Q_SIGNALS: // SIGNALS
    /**
     * Sent when new tap event has occurred.
     * @param The tap event.
     */
    void dataAvailable(const Tap&);
};

namespace local {
  typedef ::TapSensorChannelInterface TapSensor;
}

#endif
