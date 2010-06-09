/**
   @file orientationinterpreter.cpp
   @brief OrientationInterpreter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

OrientationInterpreter::OrientationInterpreter() :
        faceSink(this, &OrientationInterpreter::faceDataAvailable),
        topEdgeSink(this, &OrientationInterpreter::topEdgeDataAvailable),
        accDataSink(this, &OrientationInterpreter::accDataAvailable),
        o_(PoseData::Undefined)
{
    addSink(&faceSink, "facesink");
    addSink(&topEdgeSink, "topedgesink");
    addSink(&accDataSink, "accsink");
    addSource(&orientationSource, "orientation");
}

void OrientationInterpreter::faceDataAvailable(unsigned, const PoseData* data)
{
    face.orientation_ = data->orientation_;
}

void OrientationInterpreter::topEdgeDataAvailable(unsigned, const PoseData* data)
{
    topEdge.orientation_ = data->orientation_;
}

void OrientationInterpreter::accDataAvailable(unsigned, const AccelerationData* data)
{
    int x = data->x_;
    int y = data->y_;
    int z = data->z_;

    PoseData newPose;

    if ( (abs(x) < 300 || abs(y) < 300) &&
         abs(z) > 650 )
        newPose.orientation_ = face.orientation_;
    else
        newPose.orientation_ = topEdge.orientation_;

    if (newPose.orientation_ != o_.orientation_) {
        o_.orientation_ = newPose.orientation_;
        o_.timestamp_ = data->timestamp_;
        orientationSource.propagate(1,&o_ );
    }
}
