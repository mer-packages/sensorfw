/**
   @file magnetometersensor_i.h
   @brief Interface for MagnetometerSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

#ifndef MAGNETOMETERSENSOR_I_H
#define MAGNETOMETERSENSOR_I_H

#include <QtDBus/QtDBus>

#include "abstractsensor_i.h"
#include <datatypes/magneticfield.h>

/**
 * @brief DBus-interface for listening on magnetic field measurements.
 * 
 * Acts as a proxy class for interface \e local.MagnetometerSensor interface.
 *
 * For details of measurement process, see #MagnetometerSensorChannel.
 * An initial datarate of 1hz is set to get some output data without manual
 * adjustment.
 */
class MagnetometerSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT;
    Q_PROPERTY(MagneticField magneticField READ magneticField);

public:
    static inline const char *staticInterfaceName()
    { return "local.MagnetometerSensor"; }

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId)
    {
        // ToDo: see which arguments can be made explicit
        return new MagnetometerSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
    }

    inline MagneticField magneticField() const
    { 
        return qvariant_cast< MagneticField >(internalPropGet("magneticField"));
    }

public:
    MagnetometerSensorChannelInterface(const QString &path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static const MagnetometerSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static MagnetometerSensorChannelInterface* controlInterface(const QString& id);

public Q_SLOTS: // METHODS
    void dataReceived();
    QDBusReply<void> reset();

Q_SIGNALS: // SIGNALS
    /**
     * Sent when new measurement is available.
     * @param data Current magnetic field measurement.
     */
    void dataAvailable(const MagneticField& data);
};

namespace local {
  typedef ::MagnetometerSensorChannelInterface MagnetometerSensor;
}

#endif /* MAGNETOMETERSENSOR_I_H */
