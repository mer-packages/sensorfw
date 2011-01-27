/**
   @file tapsensor_i.h
   @brief Interface for TapSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
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
 * @brief DBus-interface for accessing device tap events.
 *
 * Acts as a proxy class for interface \e local.TapSensor.
 *
 * For details of measurement process, see #TapSensorChannel.
 * @todo Add member and method descriptions.
 */
class TapSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(TapSensorChannelInterface)

public:
    static const char* staticInterfaceName;

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId);

public:
    TapSensorChannelInterface(const QString &path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static const TapSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static TapSensorChannelInterface* controlInterface(const QString& id);

    /**
     * Request a interface to the sensor.
     * @param id Identifier string for the sensor.
     * @return Pointer to interface, or NULL on failure.
     */
    static TapSensorChannelInterface* interface(const QString& id);

protected:
    virtual bool dataReceivedImpl();

private:
    void output();

Q_SIGNALS: // SIGNALS
    /**
     * Sent when new tap event has occurred.
     * @param data The tap event.
     */
    void dataAvailable(const Tap& data);

public:

    enum TapSelection
    {
        Single = 1,
        Double,
        SingleDouble
    };

    void setTapType(TapSelection type);
    TapSelection getTapType();

private:

    QList<TapData> tapValues_;
    TapSelection type_;
    QTimer *timer;
    static const int doubleClickInteval = 500;
};

namespace local {
  typedef ::TapSensorChannelInterface TapSensor;
}

#endif
