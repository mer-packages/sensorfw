/**
   @file coordinatealignfilter.cpp
   @brief CoordinateAlignFilter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#include "coordinatealignfilter.h"

CoordinateAlignFilter::CoordinateAlignFilter() :
        Filter<TimedXyzData, CoordinateAlignFilter, TimedXyzData>(this, &CoordinateAlignFilter::filter)
{
}

void CoordinateAlignFilter::filter(unsigned, const TimedXyzData* data)
{
    TimedXyzData transformed;

    transformed.timestamp_ = data->timestamp_;

    transformed.x_ = matrix_.get(0,0)*data->x_ + matrix_.get(0,1)*data->y_ + matrix_.get(0,2)*data->z_;
    transformed.y_ = matrix_.get(1,0)*data->x_ + matrix_.get(1,1)*data->y_ + matrix_.get(1,2)*data->z_;
    transformed.z_ = matrix_.get(2,0)*data->x_ + matrix_.get(2,1)*data->y_ + matrix_.get(2,2)*data->z_;

    source_.propagate(1, &transformed);
}
