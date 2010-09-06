/**
   @file proximitysensor.h
   @brief ProximitySensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#ifndef PROXIMITY_SENSOR_CHANNEL_H
#define PROXIMITY_SENSOR_CHANNEL_H

#include "abstractsensor.h"
#include "proximitysensor_a.h"
#include "sensord/dbusemitter.h"
#include "sensord/deviceadaptor.h"
#include "datatypes/timedunsigned.h"
#include "datatypes/unsigned.h"

class Bin;
template <class TYPE> class BufferReader;

/**
 * @brief Sensor for accessing proximity sensor measurements.
 *
 * Provides measurement data from device proximity sensor.
 */
class ProximitySensorChannel :
    public AbstractSensorChannel,
    public DbusEmitter<TimedUnsigned>
{
    Q_OBJECT;
    Q_PROPERTY(Unsigned proximity READ proximity);

public:
    /**
     * Factory method for AbstractSensorChannel.
     * @return New ProximitySensorChannel as AbstractSensorChannel*.
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        ProximitySensorChannel* sc = new ProximitySensorChannel(id);
        new ProximitySensorChannelAdaptor(sc);
        return sc;
    }

    /**
     * Property for accessing the measured value.
     * @return Last measured value.
     */
    Unsigned proximity() const { return previousValue_; }


public Q_SLOTS:
    bool start();
    bool stop();

signals:
    /**
     * Sent when new measurement data has become available.
     * @param data Newly measured data.
     */
    void dataAvailable(const Unsigned& data);

protected:
    ProximitySensorChannel(const QString& id);
    ~ProximitySensorChannel();

private:
    Bin*                         filterBin_;
    Bin*                         marshallingBin_;
    DeviceAdaptor*               proximityAdaptor_;
    BufferReader<TimedUnsigned>* proximityReader_; 
    RingBuffer<TimedUnsigned>*   outputBuffer_;
    TimedUnsigned                previousValue_;

    void emitToDbus(const TimedUnsigned& value);
};

#endif // PROXIMITY_SENSOR_CHANNEL_H
