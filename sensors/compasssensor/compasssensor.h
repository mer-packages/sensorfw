/**
   @file compasssensor.h
   @brief CompassSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

#ifndef COMPASS_SENSOR_CHANNEL_H
#define COMPASS_SENSOR_CHANNEL_H

#include "abstractsensor.h"
#include "abstractchain.h"
#include "compasssensor_a.h"
#include "dbusemitter.h"
#include "orientationdata.h"
#include "datatypes/compass.h"

class Bin;
class CompassAdaptor;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief Logical sensor for measuring compass direction using magnetometer and
 *        accelerometer sensors.
 *
 * For details about the dataflow for compass, see #CompassChain.
 */
class CompassSensorChannel :
        public AbstractSensorChannel,
        public DbusEmitter<CompassData>
{
    Q_OBJECT;

    Q_PROPERTY(bool usedeclination READ declination WRITE setDeclination)
    Q_PROPERTY(quint16 declinationvalue READ declinationAngle);
    Q_PROPERTY(Compass value READ get);

public:
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        CompassSensorChannel* sc = new CompassSensorChannel(id);
        new CompassSensorChannelAdaptor(sc);

        return sc;
    }

    bool declination() const { return declinationCorrection_; }
    void setDeclination(bool enable);
    quint16 declinationAngle();

    Compass get() const { return compassData; }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    void dataAvailable(const Compass& value);

protected:
    CompassSensorChannel(const QString& id);
    ~CompassSensorChannel();

private:
    bool declinationCorrection_;
    quint16 declinationAngle_;

    CompassData compassData;

    Bin* filterBin_;
    Bin* marshallingBin_;

    AbstractChain* compassChain_;
    BufferReader<CompassData>* inputReader_;
    RingBuffer<CompassData>* outputBuffer_;

    void emitToDbus(const CompassData& value);
};

#endif
