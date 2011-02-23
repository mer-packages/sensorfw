/**
   @file samplesensor.h
   @brief SampleSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>

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

#ifndef SAMPLE_SENSOR_CHANNEL_H
#define SAMPLE_SENSOR_CHANNEL_H

#include "abstractsensor.h"
#include "abstractchain.h"

// Include the dbus adaptor and dataemitter base class
#include "samplesensor_a.h"
#include "dataemitter.h"

// Include required datatypes
#include "timedunsigned.h"

// Included in .cpp, introduced here.
class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

// Sensors inherit also DataEmitter
class SampleSensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<TimedUnsigned>
{
    Q_OBJECT;

    // Accessor function for the value for clients. Type must be based
    // on QObject and marshallable over D-Bus. Usually it just wraps the
    // POD that is used internally.
    Q_PROPERTY(Unsigned value READ get);

public:
    /**
     * Factory method for AccelerometerSensorChannel.
     * @return new AccelerometerSensorChannel as AbstractSensorChannel*.
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        SampleSensorChannel* sc = new SampleSensorChannel(id);

        // This creates the dbus adaptor - qobject parent relation
        // makes sure it gets destructed correctly.
        new SampleSensorChannelAdaptor(sc);

        return sc;
    }

    // Implementation of the accessor function.
    // Unsigned(TimedUnsigned data) exists, so this is simple
    Unsigned get() const { return Unsigned(previousSample_); }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    // Signal is used whenever we have new data.
    void dataAvailable(const Unsigned& data);

protected:
    SampleSensorChannel(const QString& id);
    ~SampleSensorChannel();

private:
    Bin*                         filterBin_;
    Bin*                         marshallingBin_;

    // Pointer to chain and a reader for it.
    AbstractChain*               sampleChain_;
    BufferReader<TimedUnsigned>* chainReader_;
    RingBuffer<TimedUnsigned>*   outputBuffer_;

    // We need to store the previous sample for accessor.
    TimedUnsigned                previousSample_;

    // Function that takes care of pushing the data to clients
    void emitData(const TimedUnsigned& value);
};

#endif
