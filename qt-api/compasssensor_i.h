/**
   @file compasssensor_i.h
   @brief Interface for CompassSensor

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

#ifndef COMPASSSENSOR_I_H
#define COMPASSSENSOR_I_H

#include <QtDBus/QtDBus>

#include "abstractsensor_i.h"
#include <datatypes/orientationdata.h>
#include <datatypes/compass.h>

/**
 * @brief DBus-interface for logical compass sensor using internal sensors.
 *
 * Acts as a proxy class \e local.CompassSensor interface.
 *
 * For details of measurement process, see #CompassSensorChannel.
 *
 * @todo Add property descriptions.
 */
class CompassSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT;

public:
    static inline const char *staticInterfaceName()
    { return "local.CompassSensor"; }

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId)
    {
        // ToDo: see which arguments can be made explicit
        return new CompassSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
    }

    Q_PROPERTY(Compass value READ get);
    Q_PROPERTY(bool usedeclination READ useDeclination WRITE setUseDeclination);
    Q_PROPERTY(int declinationvalue READ declinationValue);

    inline Compass get() const { return qvariant_cast< Compass >(internalPropGet("value")); }

    inline bool useDeclination() const { return qvariant_cast< bool >(internalPropGet("usedeclination")); }
    inline void setUseDeclination(bool enable) { internalPropSet("usedeclination", qVariantFromValue(enable)); }

    inline int declinationValue() const { return qvariant_cast< int >(internalPropGet("declinationvalue")); }

public:

    CompassSensorChannelInterface(const QString &path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static const CompassSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static CompassSensorChannelInterface* controlInterface(const QString& id);


protected:
    CompassData prevValue_;

public Q_SLOTS: // METHODS
    void dataReceived();

Q_SIGNALS: // SIGNALS

    /**
     * Sent when compass direction or calibration level has changed.
     * @param level Current compass measurement.
     */
    void dataAvailable(const Compass& value);
};

namespace local {
  typedef ::CompassSensorChannelInterface CompassSensor;
}

#endif
