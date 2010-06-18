/**
   @file orientationinterpreter.cpp
   @brief OrientationInterpreter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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
#include <sensord/logging.h>
#include <math.h>

#define DEFAULT_THRESHOLD 250
#define RADIANS_TO_DEGREES 180.0/M_PI
#define ANGLE_LIMIT 45
#define SAME_AXIS_LIMIT 5

OrientationInterpreter::OrientationInterpreter() :
        accDataSink(this, &OrientationInterpreter::accDataAvailable),
        threshold_(*this),
        topEdge(PoseData::BottomDown),
        o_(PoseData::Undefined)
{
    addSink(&accDataSink, "accsink");
    addSource(&topEdgeSource, "topedge");
    addSource(&faceSource, "face");
    addSource(&orientationSource, "orientation");

    threshold_(DEFAULT_THRESHOLD);
}

void OrientationInterpreter::accDataAvailable(unsigned, const AccelerationData* pdata)
{
    data = *pdata;

    // Check overflow
    if (overFlowCheck())
    {
        sensordLogT() << "Acc value discarded";
        return;
    }
    // calculate face
    processFace();

    // calculate topedge
    processTopEdge();

    // calculate orientation
    processOrientation();
}

bool OrientationInterpreter::overFlowCheck()
{
    int gVector = ((data.x_*data.x_ + data.y_*data.y_ + data.z_*data.z_)/1000);
    return !((gVector >= 800) && (gVector <=1250));
}

void OrientationInterpreter::processTopEdge()
{
    // Provide something on the first run (topEdge + Face)

    int rotation;
    PoseData newTopEdge = topEdge;

    // Portrait check
    rotation = round(atan((double)data.x_ / sqrt(data.y_*data.y_ + data.z_*data.z_)) * RADIANS_TO_DEGREES);
    if (abs(rotation) > ANGLE_LIMIT) {
        newTopEdge.orientation_ = (rotation>=0) ? PoseData::LeftUp : PoseData::RightUp;

        // Some threshold to switching between portrait modes
        if (topEdge.orientation_ == PoseData::LeftUp || topEdge.orientation_ == PoseData::RightUp) {
            //if (topEdge.orientation_ != newTopEdge.orientation_ && rotation < SAME_AXIS_LIMIT) {
            if (abs(rotation) < SAME_AXIS_LIMIT) {
                newTopEdge.orientation_ = topEdge.orientation_;
            }
        }

    } else {
        // Landscape check
        rotation = round(atan((double)data.y_ / sqrt(data.x_*data.x_ + data.z_*data.z_)) * RADIANS_TO_DEGREES);
        if (abs(rotation) > ANGLE_LIMIT) {
            newTopEdge.orientation_ = (rotation>=0) ? PoseData::BottomUp : PoseData::BottomDown;

            // Some threshold to switching between landscape modes
            if (topEdge.orientation_ == PoseData::BottomUp || topEdge.orientation_ == PoseData::BottomDown) {
                //if (topEdge.orientation_ != newTopEdge.orientation_ && rotation < SAME_AXIS_LIMIT) {
                if (abs(rotation) < SAME_AXIS_LIMIT) {
                    newTopEdge.orientation_ = topEdge.orientation_;
                }
            }
        }
    }

    // Propagate if changed
    if (topEdge.orientation_ != newTopEdge.orientation_) {
        topEdge.orientation_ = newTopEdge.orientation_;
        sensordLogT() << "new TopEdge value:" << topEdge.orientation_;
        topEdge.timestamp_ = data.timestamp_;
        topEdgeSource.propagate(1, &topEdge);
    }

    //~ int t = threshold_();
    //~ PoseData newTopEdge;
//~
    //~ if (abs(data.x_) >= 200 || abs(data.y_) >=200) {
        //~ if (topEdge.orientation_ == PoseData::Undefined) {
            //~ /*Change topedge away from unknown when x or y
             //~ * goes above t.*/
            //~ if ( abs(data.y_) > t &&
                    //~ (abs(data.y_) > abs(data.x_)) )
                //~ newTopEdge.orientation_ = (data.y_>0 ? PoseData::BottomUp : PoseData::BottomDown);
            //~ else if ( abs(data.x_) > t &&
                    //~ (abs(data.x_) > abs (data.y_)) )
                //~ newTopEdge.orientation_ = (data.x_>0?PoseData::LeftUp : PoseData::RightUp);
        //~ } else if (topEdge.orientation_ == PoseData::LeftUp || topEdge.orientation_ == PoseData::RightUp) {
            //~ if (abs(data.y_) > abs(data.x_) + t) {
                //~ newTopEdge.orientation_ = (data.y_ > 0?PoseData::BottomUp:PoseData::BottomDown);
            //~ } else {
                //~ newTopEdge.orientation_ = (data.x_ > 0?PoseData::LeftUp:PoseData::RightUp);
            //~ }
        //~ } else if (topEdge.orientation_ == PoseData::BottomUp || topEdge.orientation_ == PoseData::BottomDown) {
            //~ if (abs(data.x_) > abs(data.y_) + t) {
                //~ newTopEdge.orientation_ = (data.x_ > 0?PoseData::LeftUp:PoseData::RightUp);
            //~ } else {
                //~ newTopEdge.orientation_ = (data.y_ > 0?PoseData::BottomUp:PoseData::BottomDown);
            //~ }
        //~ }
    //~ } else {
        //~ newTopEdge.orientation_ = PoseData::Undefined;
    //~ }
//~
    //~ if (topEdge.orientation_ != newTopEdge.orientation_) {
        //~ topEdge.orientation_ = newTopEdge.orientation_;
        //~ sensordLogT() << "new TopEdge value:" << topEdge.orientation_;
        //~ topEdge.timestamp_ = data.timestamp_;
        //~ topEdgeSource.propagate(1, &topEdge);
    //~ }
}

void OrientationInterpreter::processFace()
{
    PoseData newFace;

    if  (abs(data.z_) >= 300)
    {
        newFace.orientation_ = ((data.z_>=0)? PoseData::FaceDown : PoseData::FaceUp);
    }

    if (face.orientation_ != newFace.orientation_)
    {
        face.orientation_ = newFace.orientation_;
        sensordLogT() << "New face value:" << face.orientation_;
        face.timestamp_ = data.timestamp_;
        faceSource.propagate(1, &face);
    }
}

void OrientationInterpreter::processOrientation()
{
    PoseData newPose;

    if ( (abs(data.x_) < 300 || abs(data.y_) < 300) && abs(data.z_) > 650 ) {
        newPose.orientation_ = face.orientation_;
    } else {
        newPose.orientation_ = topEdge.orientation_;
    }

    if (newPose.orientation_ != o_.orientation_) {
        o_.orientation_ = newPose.orientation_;
        sensordLogT() << "New orientation value:" << o_.orientation_;
        o_.timestamp_ = data.timestamp_;
        orientationSource.propagate(1, &o_);
    }
}
