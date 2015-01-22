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
#include <datatypes/compass.h>

/**
 * Client interface for accessing compass sensor.
 */
class CompassSensorChannelInterface : public AbstractSensorChannelInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(CompassSensorChannelInterface)
    Q_PROPERTY(Compass value READ get)
    Q_PROPERTY(bool usedeclination READ useDeclination WRITE setUseDeclination)
    Q_PROPERTY(int declinationvalue READ declinationValue)

public:
    /**
     * Name of the D-Bus interface for this class.
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
     * Get latest compass reading from sensor daemon.
     *
     * @return compass reading.
     */
    Compass get();

    /**
     * Returns whether the sensor is applying declination correction to the
     * output value and returning <i>true north</i>, or not applying it and
     * returning <i>magnetic north</i>.
     *
     * @return True if decliation correction is applied, false otherwise
     */
    bool useDeclination();

    /**
     * Sets whether the declination correction should be applied or not.
     *
     * @param enable If true, declination correction will be applied,
     *               if false, it will not be applied
     */
    void setUseDeclination(bool enable);

    /**
     * Returns the currently used declination correction value.
     *
     * @return Current declination value
     */
    int declinationValue();

    /**
     * Constructor.
     *
     * @param path      path.
     * @param sessionId session ID.
     */
    CompassSensorChannelInterface(const QString& path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static const CompassSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static CompassSensorChannelInterface* controlInterface(const QString& id);

    /**
     * Request an interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     */
    static CompassSensorChannelInterface* interface(const QString& id);

protected:
    virtual bool dataReceivedImpl();

Q_SIGNALS:

    /**
     * Sent when compass direction or calibration level has changed.
     *
     * @param value Current compass measurement.
     */
    void dataAvailable(const Compass& value);

private:

    bool useDeclination_;
};

namespace local {
  typedef ::CompassSensorChannelInterface CompassSensor;
}

#endif
