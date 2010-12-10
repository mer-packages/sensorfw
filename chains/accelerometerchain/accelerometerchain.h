/**
   @file accelerometerchain.h
   @brief AccelerometerChain

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

#ifndef ACCELEROMETERCHAIN_H
#define ACCELEROMETERCHAIN_H

#include "abstractsensor.h"
#include "abstractchain.h"
#include "accelerometeradaptor/accelerometeradaptor.h"
#include "coordinatealignfilter/coordinatealignfilter.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief Accelerometerchain providies raw accelerometer coordinates
 *        aligned to Nokia Standard Coordinate system.
 *
 * <b>Output buffers:</b>
 * <ul><li>\em accelerometer</li></ul>
 *
 * For direct raw data (no coordinate correction) use #AccelerometerAdaptor.
 */
class AccelerometerChain : public AbstractChain
{
    Q_OBJECT;

public:
    /**
     * Factory method for AccelerometerChain.
     * @return Pointer to new AccelerometerChain instance as AbstractChain*
     */
    static AbstractChain* factoryMethod(const QString& id)
    {
        AccelerometerChain* sc = new AccelerometerChain(id);
        return sc;
    }

public Q_SLOTS:
    bool start();
    bool stop();

protected:
    AccelerometerChain(const QString& id);
    ~AccelerometerChain();

private:

    bool setMatrixFromString(const QString str);

    static double                    aconv_[3][3];
    Bin*                             filterBin_;

    DeviceAdaptor*                   accelerometerAdaptor_;
    BufferReader<AccelerationData>*  accelerometerReader_;
    FilterBase*                      accCoordinateAlignFilter_;
    RingBuffer<AccelerationData>*    outputBuffer_;
};

#endif // ACCELEROMETERCHAIN_H
