/**
   @file orientationsensor_i.h
   @brief Interface for OrientationSensor

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

#ifndef ORIENTATIONSENSOR_I_H
#define ORIENTATIONSENSOR_I_H

#include <QtDBus/QtDBus>

#include <datatypes/unsigned.h>
#include "abstractsensor_i.h"

/**
 * @brief DBus-interface for listening on device orientation changes.
 * 
 * Acts as a proxy class for interface \e local.OrientationSensor interface.
 *
 * For details of measurement process, see #OrientationSensorChannel. 
 * 
 * @todo Add property descriptions.
 */
class OrientationSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT;

public:
    static inline const char *staticInterfaceName()
    { return "local.OrientationSensor"; }

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId)
    {
        // ToDo: see which arguments can be made explicit
        return new OrientationSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
    }

    Q_PROPERTY(Unsigned orientation READ orientation)
    Q_PROPERTY(int threshold READ threshold WRITE setThreshold);
    
    inline Unsigned orientation() const { return qvariant_cast< Unsigned >(internalPropGet("orientation")); }
    inline int threshold() const { return qvariant_cast< int >(internalPropGet("threshold")); }
    inline void setThreshold(int value) { internalPropSet("threshold", qVariantFromValue(value)); }
    
public:
    OrientationSensorChannelInterface(const QString &path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static const OrientationSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static OrientationSensorChannelInterface* controlInterface(const QString& id);

public Q_SLOTS: // METHODS
    void dataReceived();

Q_SIGNALS: // SIGNALS
    /**
     * Sent when device orientation has changed.
     * @param orientation Current device orientation. The given integer value is 
                          enumeration from PoseData::Orientation.
     */
    void orientationChanged(const Unsigned& orientation);    
};

namespace local {
  typedef ::OrientationSensorChannelInterface OrientationSensor;
}

#endif /* ORIENTATIONSENSOR_I_H */
