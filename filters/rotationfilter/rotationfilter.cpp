/**
   @file rotationfilter.cpp
   @brief RotationFilter

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

#include "rotationfilter.h"
#include <math.h>

RotationFilter::RotationFilter() :
        accelerometerDataSink_(this, &RotationFilter::interpret),
        compassDataSink_(this, &RotationFilter::updateZvalue),
        rotation_(0,0,0,0)
{
    addSink(&accelerometerDataSink_, "accelerometersink");
    addSink(&compassDataSink_, "compasssink");
    addSource(&source_, "source");
}

void RotationFilter::interpret(unsigned, const TimedXyzData* data)
{
    const int RADIANS_TO_DEGREES = 180/M_PI;

    rotation_.timestamp_ = data->timestamp_;

    // X-Rotation
    rotation_.x_ = round(atan((double)data->y_ / sqrt(data->x_ * data->x_ + data->z_ * data->z_)) * RADIANS_TO_DEGREES);
    rotation_.x_ = -rotation_.x_;

    // Y-rotation
    if (data->x_ == 0 && data->y_ == 0 && data->z_ > 0) {
        rotation_.y_ = 180;
    } else if (data->x_ == 0 && data->z_  == 0) {
        rotation_.y_ = 0;
    } else {
        rotation_.y_ = round(atan((double)data->x_ / sqrt(data->y_ * data->y_ + data->z_ * data->z_)) * RADIANS_TO_DEGREES);

        qreal theta = atan(sqrt(data->x_ * data->x_ + data->y_ * data->y_) / data->z_) * RADIANS_TO_DEGREES;
        if (theta > 0) {
            if (rotation_.y_ >= 0)
                rotation_.y_ = 180 - rotation_.y_;
            else
                rotation_.y_ = -180 - rotation_.y_;
        }
    }

    source_.propagate(1, &rotation_);
}

double RotationFilter::vectorLength(const TimedXyzData& data)
{
    return sqrt(data.x_ * data.x_ + data.y_ * data.y_ + data.z_ * data.z_);
}

void RotationFilter::updateZvalue(unsigned, const CompassData* data)
{
    rotation_.timestamp_ = data->timestamp_;

    /// Z-rotation
    /// Compass output is [0, 360), rotation is (-180, 180]
    rotation_.z_ = -1 * (data->degrees_ - 180);
}
