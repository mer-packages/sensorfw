/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com

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
 */
#ifndef COMPASSCHAIN_H
#define COMPASSCHAIN_H

#include "abstractsensor.h"
#include "abstractchain.h"
#include "coordinatealignfilter.h"
#include "deviceadaptor.h"
#include "bufferreader.h"
#include "filter.h"
#include "bin.h"

#include "orientationdata.h"
#include "timedunsigned.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

class CompassChain : public AbstractChain
{
    Q_OBJECT

    Q_PROPERTY(bool compassEnabled READ compassEnabled WRITE setCompassEnabled)
    Q_PROPERTY(quint16 declinationValue READ declinationValue)
public:

    static AbstractChain *factoryMethod(const QString& id)
    {
        CompassChain *sc = new CompassChain(id);
        return sc;
    }

    bool compassEnabled() const;
    void setCompassEnabled(bool enabled);

    quint16 declinationValue() const;

public Q_SLOTS:
    bool start();
    bool stop();
    void resetCalibration();

    // declinationAngle()
protected:
    CompassChain(const QString& id);
    ~CompassChain();

private:
    Bin* filterBin;

    AbstractChain *accelerometerChain;
    AbstractChain *magChain;

    BufferReader<AccelerationData> *accelerometerReader;
    BufferReader<CalibratedMagneticFieldData> *magReader;

    DeviceAdaptor *orientAdaptor;
    BufferReader<TimedXyzData> *orientationdataReader;


    FilterBase *compassFilter;
    FilterBase *orientationFilter;
    FilterBase *declinationFilter;

    FilterBase *downsampleFilter;
    FilterBase *avgaccFilter;

    RingBuffer<CompassData> *trueNorthBuffer;
    RingBuffer<CompassData> *magneticNorthBuffer;

    bool hasOrientationAdaptor;
};

#endif // COMPASSCHAIN_H
