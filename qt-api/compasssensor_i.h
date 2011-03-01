/**
   @file compasssensor_i.h
   @brief Interface for CompassSensor

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
    Q_OBJECT
    Q_DISABLE_COPY(CompassSensorChannelInterface)
    Q_PROPERTY(Compass value READ get)
    Q_PROPERTY(bool usedeclination READ useDeclination WRITE setUseDeclination)
    Q_PROPERTY(int declinationvalue READ declinationValue)

public:
    static const char* staticInterfaceName;

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId);

    Compass get();

    bool useDeclination();
    void setUseDeclination(bool enable);

    int declinationValue();

    CompassSensorChannelInterface(const QString& path, int sessionId);

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

    /**
     * Request a interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static CompassSensorChannelInterface* interface(const QString& id);

protected:
    virtual bool dataReceivedImpl();

Q_SIGNALS: // SIGNALS

    /**
     * Sent when compass direction or calibration level has changed.
     * @param value Current compass measurement.
     */
    void dataAvailable(const Compass& value);
};

namespace local {
  typedef ::CompassSensorChannelInterface CompassSensor;
}

#endif
