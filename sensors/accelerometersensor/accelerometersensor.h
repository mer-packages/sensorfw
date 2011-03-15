/**
   @file accelerometersensor.h
   @brief AccelerometerSensor

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

#ifndef ACCELEROMETER_SENSOR_CHANNEL_H
#define ACCELEROMETER_SENSOR_CHANNEL_H

#include "abstractsensor.h"
#include "abstractchain.h"
#include "accelerometersensor_a.h"
#include "dataemitter.h"
#include "datatypes/orientationdata.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;


/**
 * @brief Sensor providing accelerometer measurements.
 *
 * Accelerometer measurements are provided corrected to the Nokia
 * Coordinate System.
 */
class AccelerometerSensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<AccelerationData>
{
    Q_OBJECT;
    Q_PROPERTY(XYZ value READ get);

public:
    /**
     * Factory method for AccelerometerSensorChannel.
     * @return new AccelerometerSensorChannel as AbstractSensorChannel*.
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        AccelerometerSensorChannel* sc = new AccelerometerSensorChannel(id);
        new AccelerometerSensorChannelAdaptor(sc);

        return sc;
    }

    XYZ get() const { return previousSample_; }

    virtual void removeSession(int sessionId);

    virtual bool downsamplingSupported() const;

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
    AccelerometerSensorChannel(const QString& id);
    virtual ~AccelerometerSensorChannel();

private:
    static double                    aconv_[3][3];
    Bin*                             filterBin_;
    Bin*                             marshallingBin_;
    AbstractChain*                   accelerometerChain_;
    BufferReader<AccelerationData>*  accelerometerReader_;
    RingBuffer<AccelerationData>*    outputBuffer_;
    AccelerationData                 previousSample_;
    TimedXyzDownsampleBuffer         downsampleBuffer_;

    void emitData(const AccelerationData& value);
};

#endif
