/**
   @file alssensor.h
   @brief ALSSensor

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

#ifndef ALS_SENSOR_CHANNEL_H
#define ALS_SENSOR_CHANNEL_H

#include <QObject>

#include "deviceadaptor.h"
#include "abstractsensor.h"
#include "alssensor_a.h"
#include "dataemitter.h"
#include "datatypes/timedunsigned.h"
#include "datatypes/unsigned.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

#ifdef PROVIDE_CONTEXT_INFO
#include <ContextProvider>
#endif

/**
 * @brief Sensor for accessing the internal ambient light sensor measurements.
 *
 * Signals listeners whenever observed ambient light intensity level has
 * changed.
 */
class ALSSensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<TimedUnsigned>
{
    Q_OBJECT;
    Q_PROPERTY(Unsigned lux READ lux);

public:
    /**
     * Factory method for ALSSensorChannel.
     * @return New ALSSensorChannel as AbstractSensorChannel*
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        ALSSensorChannel* sc = new ALSSensorChannel(id);
        new ALSSensorChannelAdaptor(sc);

        return sc;
    }

    /**
     * Property for accessing the measured value.
     * @return Last measured value.
     */
    Unsigned lux() const { return previousValue_; }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    /**
     * Sent when a change in measured data is observed.
     * @param value Measured value.
     */
    void ALSChanged(const Unsigned& value);

protected:
    ALSSensorChannel(const QString& id);
    virtual ~ALSSensorChannel();

private:
    TimedUnsigned                 previousValue_;
    Bin*                          filterBin_;
    Bin*                          marshallingBin_;
    DeviceAdaptor*                alsAdaptor_;
    BufferReader<TimedUnsigned>*  alsReader_;
    RingBuffer<TimedUnsigned>*    outputBuffer_;

    void emitData(const TimedUnsigned& value);

#ifdef PROVIDE_CONTEXT_INFO
    ContextProvider::Service service;
    ContextProvider::Property isDarkProperty; ///< For publishing the Environment.IsDark contextProperty
    ContextProvider::Property isBrightProperty; ///< For publishing the Environment.IsBright contextProperty
    ContextProvider::Group propertyGroup;
#endif
};

#endif // ALS_SENSOR_CHANNEL_H
