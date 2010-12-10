/**
   @file accelerometersensor_i.h
   @brief Interface for AccelerometerSensor

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

#ifndef ACCELEROMETERSENSOR_I_H
#define ACCELEROMETERSENSOR_I_H

#include <QtDBus/QtDBus>
#include <QVector>
#include "abstractsensor_i.h"
#include <datatypes/xyz.h>

/**
 * @brief DBus-interface for accessing raw accelerometer values.
 *
 * AccelerometerSensorChannelInterface acts as a proxy class for interface
 * \e local.AccelerometerSensor.
 *
 * For details of measurement process, see #AccelerometerSensorChannel.
 */
class AccelerometerSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(AccelerometerSensorChannelInterface)
    Q_PROPERTY(XYZ value READ get)

public:
    /**
     * Name of the D-Bus interface for this class.
     */
    static const char* staticInterfaceName;

    /**
     * Get an instance of the class.
     * @return Pointer to new instance of the class.
     */
    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId);

    XYZ get() const;

    /**
     * Constructor.
     * @param path      path.
     * @param sessionId session id.
     */
    AccelerometerSensorChannelInterface(const QString& path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static const AccelerometerSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static AccelerometerSensorChannelInterface* controlInterface(const QString& id);

protected:
    virtual void connectNotify(const char* signal);

private:
    bool frameAvailableConnected;

public Q_SLOTS: // METHODS
    void dataReceived();

Q_SIGNALS: // SIGNALS
    /**
     * Sent when new measurement data has become available.
     * @param data New measurement data.
     */
    void dataAvailable(const XYZ& data);

    /**
     * Sent when new measurement frame has become available.
     * If app doesn't connect to this signal content of frames
     * will be sent through dataAvailable signal.
     * @param frame New measurement frame.
     */
    void frameAvailable(const QVector<XYZ>& frame);
};

namespace local {
  typedef ::AccelerometerSensorChannelInterface AccelerometerSensor;
}

#endif
