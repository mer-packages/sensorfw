/**
   @file proximitysensor_i.h
   @brief Interface for ProximitySensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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
#ifndef PROXIMITYSENSOR_I_H
#define PROXIMITYSENSOR_I_H

#include <QtDBus/QtDBus>

#include "abstractsensor_i.h"
#include <datatypes/unsigned.h>
#include <datatypes/proximity.h>

/**
 * Client interface for listening proximity sensor state changes.
 */
class ProximitySensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(ProximitySensorChannelInterface)
    Q_PROPERTY(Unsigned proximity READ proximity);
    Q_PROPERTY(Proximity proximityReflectance READ proximityReflectance);

public:
    /**
     * Get name of the D-Bus interface for this class.
     *
     * @return Name of the interface.
     */
    static const char* staticInterfaceName;

    /**
     * Create new instance of the class.
     *
     * @param id Sensor ID.
     * @param sessionId Session ID.
     * @return Pointer to new instance of the class.
     */
    static AbstractSensorChannelInterface* factoryMethod(const QString& id, int sessionId);

    /**
     * Get latest proximity reading from sensor daemon.
     *
     * @return proximity reading. Non-zero value means that object is within proximity.
     * @deprecated Use #proximityReflectance().
     */
    Unsigned proximity();

    /**
     * Get latest proximity reading from sensor daemon.
     *
     * @return proximity reading.
     */
    Proximity proximityReflectance();

    /**
     * Constructor.
     *
     * @param path      path.
     * @param sessionId session id.
     */
    ProximitySensorChannelInterface(const QString& path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use #interface(const QString&) instead.
     */
    static const ProximitySensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use #interface(const QString&) instead.
     */
    static ProximitySensorChannelInterface* controlInterface(const QString& id);

    /**
     * Request an interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     */
    static ProximitySensorChannelInterface* interface(const QString& id);

protected:
    virtual bool dataReceivedImpl();

Q_SIGNALS:
    /**
     * Sent when new measurement data has become available.
     * Value in the passed data contains boolean information is the
     * proximity sensor blocked or not.
     *
     * @param data New measurement data.
     * @deprecated Use #reflectanceDataAvailable(const Proximity&).
     */
    void dataAvailable(const Unsigned& data);

    /**
     * Sent when new measurement data has become available.
     *
     * @param data New measurement data.
     */
    void reflectanceDataAvailable(const Proximity& data);
};

namespace local {
  typedef ::ProximitySensorChannelInterface ProximitySensor;
}

#endif
