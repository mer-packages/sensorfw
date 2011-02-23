/**
   @file gyroscopesensor.h
   @brief GyroscopeSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>

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

#ifndef GYROSCOPE_SENSOR_CHANNEL_H
#define GYROSCOPE_SENSOR_CHANNEL_H

#include "abstractsensor.h"
#include "deviceadaptor.h"

#include "gyroscopesensor_a.h"
#include "dataemitter.h"

#include "datatypes/orientationdata.h"
#include "datatypes/xyz.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

class GyroscopeSensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<TimedXyzData>
{
    Q_OBJECT;
    Q_PROPERTY(XYZ value READ get);

public:
    /**
     * Factory method for GyroscopeSensorChannel.
     * @return new GyroscopeSensorChannel as AbstractSensorChannel*.
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        GyroscopeSensorChannel* sc = new GyroscopeSensorChannel(id);
        new GyroscopeSensorChannelAdaptor(sc);

        return sc;
    }

    XYZ get() const { return previousSample_; }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    void dataAvailable(const XYZ& data);

protected:
    GyroscopeSensorChannel(const QString& id);
    ~GyroscopeSensorChannel();

private:
    Bin*                         filterBin_;
    Bin*                         marshallingBin_;

    DeviceAdaptor*                     gyroscopeAdaptor_;
    BufferReader<TimedXyzData>* gyroscopeReader_;
    RingBuffer<TimedXyzData>*   outputBuffer_;

    TimedXyzData                previousSample_;

    void emitData(const TimedXyzData& value);
};

#endif // GYROSCOPE_SENSOR_CHANNEL_H
