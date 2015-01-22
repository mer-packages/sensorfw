/**
   @file tapsensor_i.h
   @brief Interface for TapSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
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

#ifndef TAPSENSOR_I_H
#define TAPSENSOR_I_H

#include <QtDBus/QtDBus>

#include "abstractsensor_i.h"
#include "datatypes/tap.h"
#include "datatypes/tapdata.h"
#include <QList>
#include <QTimer>


/**
 * Client interface for accessing accelerometer based tap events.
 */
class TapSensorChannelInterface : public AbstractSensorChannelInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(TapSensorChannelInterface)

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
     * Constructor.
     *
     * @param path      path.
     * @param sessionId session ID.
     */
    TapSensorChannelInterface(const QString &path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static const TapSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static TapSensorChannelInterface* controlInterface(const QString& id);

    /**
     * Request an interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     */
    static TapSensorChannelInterface* interface(const QString& id);

    /**
     * Tap type selection
     */
    enum TapSelection
    {
        Single = 1,  /**< Only listen single taps. */
        Double,      /**< Only listen double taps. */
        SingleDouble /**< Listen both single and double taps. */
    };

    /**
     * Set type of taps to be listened for.
     *
     * @param type type of tap to be listened for.
     */
    void setTapType(TapSelection type);

    /**
     * Get type of taps to be listened for.
     *
     * @return type of taps to be listened for.
     */
    TapSelection getTapType();

protected:
    virtual bool dataReceivedImpl();

private Q_SLOTS:
    void output();

Q_SIGNALS:
    /**
     * Sent when new tap event has occurred.
     *
     * @param data The tap event.
     */
    void dataAvailable(const Tap& data);

private:

    QList<TapData> tapValues_; /**< buffer of received tap values. */
    TapSelection type_; /**< tap type to listen for. */
    QTimer *timer_; /**< timer for doubletap detection. */
    static const int doubleClickInteval = 500; /**< doubletap recognition window */
};

namespace local {
  typedef ::TapSensorChannelInterface TapSensor;
}

#endif
