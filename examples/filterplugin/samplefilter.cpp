/**
   @file samplefilter.cpp
   @brief SampleFilter

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

#include "samplefilter.h"

SampleFilter::SampleFilter() :
        Filter<TimedUnsigned, SampleFilter, TimedUnsigned>(this, &SampleFilter::filter)
{
}

void SampleFilter::filter(unsigned, const TimedUnsigned* data)
{
    TimedUnsigned transformed;

    // Usually you want to keep the timestamp of the original data, as
    // one is likely to be interested in the time that the action
    // happened. Apply common sense.
    transformed.timestamp_ = data->timestamp_;

    // Do something for the value.
    transformed.value_ = data->value_ * data->value_;

    // Propagate the altered sample to outputs
    source_.propagate(1, &transformed);
}
