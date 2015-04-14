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


#ifndef MAGCALIBRATIONCHAIN_H
#define MAGCALIBRATIONCHAIN_H

#include "abstractsensor.h"
#include "abstractchain.h"
#include "magcoordinatealignfilter.h"
#include "deviceadaptor.h"
#include "bufferreader.h"
#include "filter.h"
#include "bin.h"

#include "orientationdata.h"
#include "timedunsigned.h"

/*
 * // property
 *
 * //// MagCalibrationChain
 * calibratedmagnetometerdata
 * resetCalibration
 **/
class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief MagCalibrationChain
 */
class MagCalibrationChain : public AbstractChain
{
    Q_OBJECT

public:
    /**
     * Factory method for MagCalibrationChain.
     * @return Pointer to new MagCalibrationChain instance as AbstractChain*
     */
    static AbstractChain *factoryMethod(const QString& id)
    {
        MagCalibrationChain *sc = new MagCalibrationChain(id);
        return sc;
    }

public Q_SLOTS:
    bool start();
    bool stop();
    void resetCalibration();

protected:
    MagCalibrationChain(const QString& id);
    ~MagCalibrationChain();

private:
    bool setMatrixFromString(const QString& str);
    double aconv_[3][3];

    Bin* filterBin;
    DeviceAdaptor *magAdaptor;

    BufferReader<CalibratedMagneticFieldData>  *magReader; //pusher/producer

    FilterBase *magCalFilter;
    FilterBase *magScaleFilter;

    FilterBase *magCoordinateAlignFilter_;
    RingBuffer<CalibratedMagneticFieldData> *calibratedMagnetometerData; //consumer
    bool needsCalibration;
};

#endif // MAGCALIBRATIONCHAIN_H
