/**
   @file orientationsensor.h
   @brief OrientationSensor

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

#ifndef ORIENTATION_SENSOR_CHANNEL_H
#define ORIENTATION_SENSOR_CHANNEL_H

#include "abstractsensor.h"
#include "abstractchain.h"
#include "orientationsensor_a.h"
#include "dataemitter.h"
#include "datatypes/orientationdata.h"
#include "datatypes/posedata.h"
#include "datatypes/unsigned.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief Sensor for accessing device orientation.
 *
 * Provides device orientation based on the direction of acceleration vector.
 * Threshold value (mG) is used to control the sensitivity of change from one
 * orientation into another. See #OrientationInterpreter for details on threshold.
 */
class OrientationSensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<PoseData>
{
    Q_OBJECT;

    Q_PROPERTY(Unsigned orientation READ orientation);
    //Q_PROPERTY(int threshold READ threshold WRITE setThreshold);
public:

    /**
     * Factory method for OrientationSensorChannel.
     * @return New OrientationSensorChannel as AbstractSensorChannel*
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        OrientationSensorChannel* sc = new OrientationSensorChannel(id);
        new OrientationSensorChannelAdaptor(sc);

        return sc;
    }

    /**
    * Property method returning current orientation.
    * @return Current orientation.
    */
    Unsigned orientation() const
    {
        TimedUnsigned o = orientationChain_->property("orientation").value<TimedUnsigned>();
        return Unsigned(o);
    }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    /**
     * Sent whenever orientation interpretation has changed.
     * @param orientation New orientation.
     */
    void orientationChanged(const int& orientation);

protected:
    OrientationSensorChannel(const QString& id);
    virtual ~OrientationSensorChannel();

private:
    PoseData                         prevOrientation;
    Bin*                             filterBin_;
    Bin*                             marshallingBin_;

    AbstractChain*                   orientationChain_;

    BufferReader<PoseData>*   orientationReader_;

    RingBuffer<PoseData>*            outputBuffer_;

    /**
     * Emits new device orientation through DBus.
     * @param value Orientation value to emit.
     */
    void emitToDbus(const PoseData& value);
};

#endif // ORIENTATION_SENSOR_CHANNEL_H
