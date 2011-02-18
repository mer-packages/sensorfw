/**
   @file samplefilter.h
   @brief SampleFilter

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

#ifndef SAMPLEFILTER_H
#define SAMPLEFILTER_H

// Include base
#include "filter.h"

// Include datatypes for input and output.
#include "timedunsigned.h"

// This is a simplest possible filter, with one input and one output.
// In case you wish to create more complex filters, with several inputs
// and outputs, have a look at FilterBase or
// OrientationInterpreterFilter.

// You must provide InputType and outputType to get buffers of correct
// type automatically. Here, input and output are of the same type.
class SampleFilter : public QObject, public Filter<TimedUnsigned, SampleFilter, TimedUnsigned>
{
    Q_OBJECT;

public:
    /**
     * Factory method.
     * @return New SampleFilter instance as FilterBase*.
     */
    static FilterBase* factoryMethod() {
        return new SampleFilter;
    }

protected:
    /**
     * Constructor.
     */
    SampleFilter();

private:
    // The actual filtering function can be called anything, as long
    // as it matches the name in class constructor.
    // The first parameter stands for the number of samples available
    // from the second parameter. Usually 1.
    void filter(unsigned, const TimedUnsigned*);
};

#endif
