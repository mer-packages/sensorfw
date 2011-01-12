/**
   @file rotationsensor.h
   @brief RotationSensor

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

#ifndef ROTATION_SENSOR_CHANNEL_H
#define ROTATION_SENSOR_CHANNEL_H

#include "abstractsensor.h"
#include "abstractchain.h"
#include "rotationsensor_a.h"
#include "dbusemitter.h"
#include "datatypes/orientationdata.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief Sensor providing device rotation around axes.
 */
class RotationSensorChannel :
        public AbstractSensorChannel,
        public DbusEmitter<TimedXyzData>
{
    Q_OBJECT;
    Q_PROPERTY(XYZ rotation READ rotation);
    Q_PROPERTY(bool hasZ READ hasZ);

public:
    /**
     * Factory method for RotationSensorChannel.
     * @return new RotationSensorChannel as AbstractSensorChannel*.
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        RotationSensorChannel* sc = new RotationSensorChannel(id);
        new RotationSensorChannelAdaptor(sc);

        return sc;
    }

    XYZ rotation() const
    {
        return XYZ(prevRotation_);
    }

    bool hasZ() const
    {
        return hasZ_;
    }

    virtual unsigned int interval() const;
    virtual bool setInterval(unsigned int value, int sessionId);

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    /**
     * Sent when new measurement data has become available.
     * @param data Newly measured data.
     */
    void dataAvailable(const XYZ& data);

protected:
    RotationSensorChannel(const QString& id);
    virtual ~RotationSensorChannel();

private:
    Bin*                         filterBin_;
    Bin*                         marshallingBin_;
    AbstractChain*               accelerometerChain_;
    AbstractChain*               compassChain_;
    BufferReader<TimedXyzData>*  accelerometerReader_;
    BufferReader<CompassData>*   compassReader_;
    FilterBase*                  rotationFilter_;
    RingBuffer<TimedXyzData>*    outputBuffer_;
    TimedXyzData                 prevRotation_;
    bool                         hasZ_;

    void emitToDbus(const TimedXyzData& value);
};

#endif // ROTATION_SENSOR_CHANNEL_H
