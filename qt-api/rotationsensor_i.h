/**
   @file rotationsensor_i.h
   @brief Interface for RotationSensor

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

#ifndef ROTATIONSENSOR_I_H
#define ROTATIONSENSOR_I_H

#include <QtDBus/QtDBus>
#include <QVector>

#include "abstractsensor_i.h"
#include <datatypes/xyz.h>

/**
 * Client interface for listening device rotation changes.
 */
class RotationSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(RotationSensorChannelInterface)
    Q_PROPERTY(XYZ rotation READ rotation)
    Q_PROPERTY(bool hasZ READ hasZ)

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
     * Get latest rotation reading from sensor daemon.
     *
     * @return rotation reading.
     */
    XYZ rotation();

    /**
     * Does reported readings include Z coordinate.
     *
     * @return Does reported readings include Z coordinate.
     */
    bool hasZ();

    /**
     * Constructor.
     *
     * @param path      path.
     * @param sessionId session id.
     */
    RotationSensorChannelInterface(const QString& path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use #interface(const QString&) instead.
     */
    static const RotationSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use #interface(const QString&) instead.
     */
    static RotationSensorChannelInterface* controlInterface(const QString& id);

    /**
     * Request an interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     */
    static RotationSensorChannelInterface* interface(const QString& id);

protected:
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    virtual void connectNotify(const char* signal);
#else
    virtual void connectNotify(const QMetaMethod &signal);
#endif

    virtual bool dataReceivedImpl();

private:
    bool frameAvailableConnected; /**< has applicaiton connected slot for frameAvailable signal. */

Q_SIGNALS:
    /**
     * Sent when device rotation has changed.
     *
     * @param data Current device rotation.
     */
    void dataAvailable(const XYZ& data);

    /**
     * Sent when new measurement frame has become available.
     * If app doesn't connect to this signal content of frames
     * will be sent through dataAvailable signal.
     *
     * @param frame New measurement frame.
     */
    void frameAvailable(const QVector<XYZ>& frame);
};

namespace local {
  typedef ::RotationSensorChannelInterface RotationSensor;
}

#endif /* ROTATIONSENSOR_I_H */
