/**
   @file orientationinterpreter.cpp
   @brief OrientationInterpreterFilter

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
#include "orientationinterpreter.h"

#include <QDebug>
#include <QVariant>

OrientationInterpreter::OrientationInterpreter() :
        Filter<TimedXyzData, OrientationInterpreter, PoseData>(this, &OrientationInterpreter::interpret),
        threshold_(*this),
        newAxis_(0),
        prevAxis_(0)
{
    threshold_(DEFAULT_THRESHOLD);
}

void OrientationInterpreter::interpret(unsigned, const TimedXyzData* data)
{
    // Pick maximum component axis and strength
    double m[3] = { data->x_, data->y_, data->z_ };
    newAxis_ = 0;
    if (abs(data->x_) < abs(data->y_)) {
        newAxis_ = 1;
    }
    if ( abs(m[newAxis_]) < abs(data->z_)) {
        newAxis_ = 2;
    }

    // Pick the face (relies on order of enumeration)
    newPose.orientation_ = (PoseData::Orientation)(newAxis_*2 + (m[newAxis_]>0?0:1) + 1);

    if (pose.orientation_ != newPose.orientation_) {
        // Axis direction changed directly or difference larger than threshold
        if ((prevAxis_ == newAxis_) || (abs(abs(m[newAxis_]) - abs(m[prevAxis_])) > threshold_())) {
            pose.orientation_ = newPose.orientation_;
            prevAxis_ = newAxis_;
        }
    }
    pose.timestamp_ = data->timestamp_;
    source_.propagate(1, &pose);
}
