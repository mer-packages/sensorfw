/**
   @file tapsensor.h
   @brief TapSensor

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

#ifndef TAP_SENSOR_CHANNEL_H
#define TAP_SENSOR_CHANNEL_H

#include <QObject>

#include "abstractsensor.h"
#include "tapsensor_a.h"
#include "sensord/dbusemitter.h"
#include "datatypes/tapdata.h"
#include "deviceadaptor.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief Sensor for accessing device tap events.
 *
 * Tap events are detected by the accelerometer driver, and propagated here through
 * TapAdaptor. The whole logic of recognising tap events is embedded in the driver
 * itself.
 *
 * In the future this class might need to contain methods for driver parameter
 * setup.
 */
class TapSensorChannel :
    public AbstractSensorChannel,
    public DbusEmitter<TapData>
{
    Q_OBJECT;

public:
    /**
     * Factory method for TapSensorChannel.
     * @return New TapSensorChannel as AbstractSensorChannel*.
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        TapSensorChannel* sc = new TapSensorChannel(id);
        new TapSensorChannelAdaptor(sc);

        return sc;
    }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    /**
     * Sent when a tap event has occurred.
     * @param tap The occurred event.
     */
    void dataAvailable(const Tap& tap);

protected:
    TapSensorChannel(const QString& id);
    ~TapSensorChannel();

private:
    Bin*                   filterBin_;
    Bin*                   marshallingBin_;
    DeviceAdaptor*         tapAdaptor_;
    BufferReader<TapData>* tapReader_;
    RingBuffer<TapData>*   outputBuffer_;

    void emitToDbus(const TapData& tapData);
};

#endif // TAP_SENSOR_CHANNEL_H
