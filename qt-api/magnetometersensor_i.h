/**
   @file magnetometersensor_i.h
   @brief Interface for MagnetometerSensor

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

#ifndef MAGNETOMETERSENSOR_I_H
#define MAGNETOMETERSENSOR_I_H

#include <QtDBus/QtDBus>
#include <QVector>

#include "abstractsensor_i.h"
#include <datatypes/magneticfield.h>

/**
 * Client interface for accessing magnetometer sensor.
 */
class MagnetometerSensorChannelInterface : public AbstractSensorChannelInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(MagnetometerSensorChannelInterface)
    Q_PROPERTY(MagneticField magneticField READ magneticField)

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
     * Get latest magnetometer reading from sensor daemon.
     *
     * @return magnetometer reading.
     */
    MagneticField magneticField();

    /**
     * Constructor.
     *
     * @param path      path.
     * @param sessionId session id.
     */
    MagnetometerSensorChannelInterface(const QString& path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static const MagnetometerSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static MagnetometerSensorChannelInterface* controlInterface(const QString& id);

    /**
     * Request an interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     */
    static MagnetometerSensorChannelInterface* interface(const QString& id);

protected:
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    virtual void connectNotify(const char* signal);
#else
    virtual void connectNotify(const QMetaMethod & signal);
#endif
    virtual bool dataReceivedImpl();

private:
    bool frameAvailableConnected; /**< has applicaiton connected slot for frameAvailable signal. */

public Q_SLOTS:
    /**
     * Reset magnetometer calibration to 0.
     *
     * @return DBus reply.
     */
    QDBusReply<void> reset();

Q_SIGNALS:
    /**
     * Sent when new measurement is available.
     *
     * @param data Current magnetic field measurement.
     */
    void dataAvailable(const MagneticField& data);

    /**
     * Sent when new measurement frame has become available.
     * If app doesn't connect to this signal content of frames
     * will be sent through dataAvailable signal.
     *
     * @param frame New measurement frame.
     */
    void frameAvailable(const QVector<MagneticField>& frame);
};

namespace local {
  typedef ::MagnetometerSensorChannelInterface MagnetometerSensor;
}

#endif /* MAGNETOMETERSENSOR_I_H */
