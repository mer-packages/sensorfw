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
    // TODO: Thread safety here
    rotation_.timestamp_ = data->timestamp_;
    //~ double vectorLength = sqrt(data->x_*data->x_ + data->y_*data->y_ + data->z_*data->z_);

    /// X-rotation
    TimedXyzData axis(0,0,1,0); /// Y-axis
    if ((data->y_ != 0) || (data->z_ != 0)) {
        TimedXyzData projection(0,0,data->y_,data->z_); /// Projection to Y/Z-plane
        rotation_.x_ = (int)round(acos(dotProduct(projection, axis)/vectorLength(projection))*180/M_PI);

        rotation_.x_ -= 90; /// Set 0 angle

    } else {
        rotation_.x_ = 0;
    }

    /// Y-rotation

    /// get the rotated z-axis as unit vector. (z = acceleration)
    //~ double glength = vectorLength(TimedXyzData(0, data->x_, 0, data->z_));
    axis.y_ = 0;
    axis.x_ = -1; //100000*(data->x_ / glength);
    //~ axis.z_ = 100000*(data->z_ / glength);

    //~ TimedXyzData projection(0,data->x_,0,data->z_); /// Projection to X/Z-plane
    //~ rotation_.y_ = (int)round(acos(dotProduct(projection, axis)/vectorLength(projection))*180/M_PI);
    //~ qDebug() << "[Z-AXIS] (" << axis.x_ << ", 0, " << axis.z_ << ")";

    if ((data->x_ != 0)  || (data->z_ != 0)) {
        TimedXyzData projection(0,data->x_,0,data->z_); /// Projection to X/Z-plane
        rotation_.y_ = (int)round(acos(dotProduct(projection, axis)/vectorLength(projection))*180/M_PI);
 
        rotation_.y_ -= 90; /// set 0 angle

        if (rotation_.y_ >= 0 && data->z_ >= 0) {
            rotation_.y_ = 90 + (90-rotation_.y_);
        }

        if (rotation_.y_ < 0 && data->z_ >= 0) {
            rotation_.y_ = -90 - (90 + rotation_.y_);
        }
    } else {
        rotation_.y_ = 0;
    }

    //~ rotation_.x_ = (int)round(acos(dotProduct(*data, TimedXyzData(0,1,0,0))/vectorLength)*180/M_PI);
    //~ rotation_.y_ = (int)round(acos(dotProduct(*data, TimedXyzData(0,0,1,0))/vectorLength)*180/M_PI);
    //~ rotation_.z_ = (int)round(acos(dotProduct(*data, TimedXyzData(0,0,0,1))/vectorLength)*180/M_PI);

    source_.propagate(1, &rotation_);
}

double RotationFilter::vectorLength(const TimedXyzData& data)
{
    return sqrt(data.x_*data.x_ + data.y_*data.y_ + data.z_*data.z_);
}

void RotationFilter::updateZvalue(unsigned, const CompassData* data)
{
    // TODO: Thread safety here...
    rotation_.timestamp_ = data->timestamp_;

    /// Z-rotation
    /// Compass output is [0, 360), rotation is (-180, 180]
    rotation_.z_ = -1*(data->degrees_ - 180);
    
    source_.propagate(1, &rotation_);
}
