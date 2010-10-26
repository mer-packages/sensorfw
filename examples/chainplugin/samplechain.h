/**
   @file samplechain.h
   @brief SampleChain

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

#ifndef SAMPLECHAIN_H
#define SAMPLECHAIN_H

// Include basetypes that we need to be aware of
#include "deviceadaptor.h"
#include "abstractsensor.h"
#include "abstractchain.h"

// Include any used datatypes
#include "datatypes/timedunsigned.h"

// Definition tricks, included from .cpp
class Bin;
template <class TYPE> class BufferReader;
class FilterBase;


class SampleChain : public AbstractChain
{
    Q_OBJECT;

public:
    /**
     * Factory method for SampleChain.
     * @return Pointer to new SampleChain instance as AbstractChain*
     */
    static AbstractChain* factoryMethod(const QString& id)
    {
        SampleChain* sc = new SampleChain(id);
        return sc;
    }

public Q_SLOTS:
    bool start();
    bool stop();

protected:
    SampleChain(const QString& id);
    ~SampleChain();

private:
    // Bins contain a set of filters
    Bin*                          filterBin_;

    // We have one source
    DeviceAdaptor*                sampleAdaptor_;

    // We need a reader for the source
    BufferReader<TimedUnsigned>*  adaptorReader_;

    // We have one filter
    FilterBase*                   sampleFilter_;

    // ...and one output buffer.
    RingBuffer<TimedUnsigned>*    outputBuffer_;
};

#endif
