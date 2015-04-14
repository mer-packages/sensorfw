/**
   @file magnetometersensor.h
   @brief MagnetometerSensor

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

#ifndef MAGNETOMETER_SENSOR_CHANNEL_H
#define MAGNETOMETER_SENSOR_CHANNEL_H

#include "abstractsensor.h"
#include "abstractchain.h"
#include "magnetometersensor_a.h"
#include "dataemitter.h"
#include "deviceadaptor.h"
#include "datatypes/orientationdata.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief Sensor providing magnetic field measurements.
 */
class MagnetometerSensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<CalibratedMagneticFieldData>
{
    Q_OBJECT;
    Q_PROPERTY(MagneticField magneticField READ magneticField);

public:
    /**
     * Factory method for MagnetometerSensorChannel.
     * @return new MagnetometerSensorChannel as AbstractSensorChannel*.
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        MagnetometerSensorChannel* sc = new MagnetometerSensorChannel(id);
        new MagnetometerSensorChannelAdaptor(sc);

        return sc;
    }

    Q_INVOKABLE void resetCalibration();

    MagneticField magneticField() const
    {
        return MagneticField(prevMeasurement_);
    }

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
    void dataAvailable(const MagneticField& data);
    void internalData(const MagneticField& data);

protected:
    MagnetometerSensorChannel(const QString& id);
    virtual ~MagnetometerSensorChannel();

    virtual bool setDataRange(const DataRange& range, int sessionId);

private:
    Bin*                                       filterBin_;
    Bin*                                       marshallingBin_;
    AbstractChain*                             magChain_;
    FilterBase*                                scaleFilter_;
    BufferReader<CalibratedMagneticFieldData>* magnetometerReader_;
    RingBuffer<CalibratedMagneticFieldData>*   outputBuffer_;
    CalibratedMagneticFieldData                prevMeasurement_;
    int                                        scaleCoefficient_;
    MagneticFieldDownsampleBuffer              downsampleBuffer_;

    void emitData(const CalibratedMagneticFieldData& value);
};

#endif // MAGNETOMETER_SENSOR_CHANNEL_H
