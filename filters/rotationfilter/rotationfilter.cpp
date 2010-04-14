/**
   @file rotationfilter.cpp
   @brief RotationFilter

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

#include "rotationfilter.h"

#include <QVariant>
#include <math.h>

RotationFilter::RotationFilter() :
        Filter<TimedXyzData, RotationFilter, TimedXyzData>(this, &RotationFilter::interpret),
        rotation_(0,0,0,0) {}

void RotationFilter::interpret(unsigned, const TimedXyzData* data)
{
    rotation_.timestamp_ = data->timestamp_;
    double vectorLength = sqrt(data->x_*data->x_ + data->y_*data->y_ + data->z_*data->z_);

    rotation_.x_ = (int)round(acos(dotProduct(*data, TimedXyzData(0,1,0,0))/vectorLength)*180/M_PI);
    rotation_.y_ = (int)round(acos(dotProduct(*data, TimedXyzData(0,0,1,0))/vectorLength)*180/M_PI);
    rotation_.z_ = (int)round(acos(dotProduct(*data, TimedXyzData(0,0,0,1))/vectorLength)*180/M_PI);
    
    source_.propagate(1, &rotation_);
}
