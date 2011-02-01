/**
   @file orientationinterpreter.cpp
   @brief OrientationInterpreter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>

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
#include "logging.h"
#include <math.h>
#include <stdlib.h>
#include "config.h"

const int OrientationInterpreter::DEFAULT_THRESHOLD = 250;
const float OrientationInterpreter::RADIANS_TO_DEGREES = 180.0/M_PI;
const int OrientationInterpreter::ANGLE_LIMIT = 45;
const int OrientationInterpreter::SAME_AXIS_LIMIT = 5;
const int OrientationInterpreter::OVERFLOW_MIN = 800;
const int OrientationInterpreter::OVERFLOW_MAX = 1250;
const int OrientationInterpreter::THRESHOLD_LANDSCAPE = 25;
const int OrientationInterpreter::THRESHOLD_PORTRAIT = 20;
const int OrientationInterpreter::DISCARD_TIME = 750000;
const int OrientationInterpreter::AVG_BUFFER_MAX_SIZE = 10;
typedef PoseData (OrientationInterpreter::*ptrFUN)(int);

OrientationInterpreter::OrientationInterpreter() :
        accDataSink(this, &OrientationInterpreter::accDataAvailable),
        threshold_(*this),
        topEdge(PoseData::Undefined),
        face(PoseData::Undefined),
        previousFace(PoseData::Undefined),
        o_(PoseData::Undefined)

{
    addSink(&accDataSink, "accsink");
    addSource(&topEdgeSource, "topedge");
    addSource(&faceSource, "face");
    addSource(&orientationSource, "orientation");

    threshold_(DEFAULT_THRESHOLD);

    minlimit = Config::configuration()->value("orientation_overflow_min",QVariant(OVERFLOW_MIN)).toInt();
    maxlimit = Config::configuration()->value("orientation_overflow_max",QVariant(OVERFLOW_MAX)).toInt();

    angleThresholdPortrait = Config::configuration()->value("orientation_threshold_portrait",QVariant(THRESHOLD_PORTRAIT)).toInt();
    angleThresholdLandscape = Config::configuration()->value("orientation_threshold_landscape",QVariant(THRESHOLD_LANDSCAPE)).toInt();

    discardTime = Config::configuration()->value("orientation_discard_time", QVariant(DISCARD_TIME)).toUInt();
}

void OrientationInterpreter::accDataAvailable(unsigned, const AccelerationData* pdata)
{
    data = *pdata;

    // Check overflow
    if (overFlowCheck())
    {
        sensordLogT() << "Acc value discarded due to over/underflow";
        return;
    }

    // Append new value to buffer
    dataBuffer.append(data);

    // Limit buffer size to N samples
    // Avoids bloating when timestamps don't make sense
    if (dataBuffer.count() > AVG_BUFFER_MAX_SIZE)
    {
        dataBuffer.erase(dataBuffer.begin(), dataBuffer.end() - 10);
    }

    // Clear old values from buffer.
    while (dataBuffer.count() > 1 && (data.timestamp_ - dataBuffer.first().timestamp_ > discardTime))
    {
        dataBuffer.removeFirst();
    }

    long x = 0;
    long y = 0;
    long z = 0;
    foreach (const AccelerationData& sample, dataBuffer)
    {
        // Dummy average calculations, optimise if a neater method
        // is figured out.
        x += sample.x_;
        y += sample.y_;
        z += sample.z_;
    }

    data.x_ = x / dataBuffer.count();
    data.y_ = y / dataBuffer.count();
    data.z_ = z / dataBuffer.count();

    // calculate topedge
    processTopEdge();

    // calculate face
    processFace();

    // calculate orientation
    processOrientation();
}

bool OrientationInterpreter::overFlowCheck()
{
    int gVector = ((data.x_*data.x_ + data.y_*data.y_ + data.z_*data.z_)/1000);
    return !((gVector >= minlimit) && (gVector <=maxlimit));
}

int OrientationInterpreter::orientationCheck(const AccelerationData data,  OrientationMode mode) const
{
    if (mode == OrientationInterpreter::Landscape)
        return round(atan((double)data.x_ / sqrt(data.y_*data.y_ + data.z_*data.z_)) * RADIANS_TO_DEGREES);
    else
        return round(atan((double)data.y_ / sqrt(data.x_*data.x_ + data.z_*data.z_)) * RADIANS_TO_DEGREES);
}


PoseData OrientationInterpreter::rotateToPortrait(int rotation)
{
    PoseData newTopEdge = PoseData::Undefined;
    newTopEdge.orientation_ = (rotation>=0) ? PoseData::BottomUp : PoseData::BottomDown;

    // Some threshold to switching between portrait modes
    if (topEdge.orientation_ == PoseData::BottomUp || topEdge.orientation_ == PoseData::BottomDown)
    {
        if (abs(rotation) < SAME_AXIS_LIMIT)
        {
            newTopEdge.orientation_ = topEdge.orientation_;
        }
    }

    return newTopEdge;
}

PoseData OrientationInterpreter::rotateToLandscape(int rotation)
{

    PoseData newTopEdge = PoseData::Undefined;
    newTopEdge.orientation_ = (rotation>=0) ? PoseData::LeftUp : PoseData::RightUp;
    // Some threshold to switching between landscape modes
    if (topEdge.orientation_ == PoseData::LeftUp || topEdge.orientation_ == PoseData::RightUp)
    {
        if (abs(rotation) < SAME_AXIS_LIMIT)
        {
            newTopEdge.orientation_ = topEdge.orientation_;
        }
    }

    return newTopEdge;

}

PoseData OrientationInterpreter::orientationRotation (const AccelerationData data, OrientationMode mode, PoseData (OrientationInterpreter::*ptrFUN)(int))
{
    int rotation = orientationCheck(data, mode);
    int threshold = (mode == OrientationInterpreter::Portrait) ? angleThresholdPortrait : angleThresholdLandscape;
    //if rotation is bigger than the threshold, then rotate using the function passed
    PoseData newTopEdge = (abs(rotation) > threshold) ? (this->*ptrFUN)(rotation) : PoseData::Undefined;
    return newTopEdge;
}

void OrientationInterpreter::processTopEdge()
{
    PoseData newTopEdge = PoseData::Undefined;
    ptrFUN rotator;
    OrientationMode mode;

    if (topEdge.orientation_ == PoseData::BottomUp || topEdge.orientation_ == PoseData::BottomDown)
    {
        mode = OrientationInterpreter::Portrait;
        rotator = &OrientationInterpreter::rotateToPortrait;
    } else {
        mode = OrientationInterpreter::Landscape;
        rotator = &OrientationInterpreter::rotateToLandscape;
    }

    newTopEdge = orientationRotation(data, mode, rotator);

    if (newTopEdge.orientation_ == PoseData::Undefined) //not rotate yet, then check for the other threshold
    {
        mode = (mode == OrientationInterpreter::Portrait) ? (OrientationInterpreter::Landscape) : (OrientationInterpreter::Portrait);
        rotator = (rotator == (&OrientationInterpreter::rotateToPortrait)) ? (&OrientationInterpreter::rotateToLandscape) : (&OrientationInterpreter::rotateToPortrait);
        newTopEdge = orientationRotation(data, mode, rotator);
    }

    // Propagate if changed
    if (topEdge.orientation_ != newTopEdge.orientation_)
    {
        topEdge.orientation_ = newTopEdge.orientation_;
        sensordLogT() << "new TopEdge value:" << topEdge.orientation_;
        topEdge.timestamp_ = data.timestamp_;
        topEdgeSource.propagate(1, &topEdge);
    }
}

void OrientationInterpreter::processFace()
{
    if (abs(data.z_) >= 300)
    {
        PoseData newFace;
        newFace.orientation_ = ((data.z_ >= 0) ? PoseData::FaceDown : PoseData::FaceUp);

        if (newFace.orientation_ == PoseData::FaceDown)
        {
            if (topEdge.orientation_ != PoseData::Undefined)
            {
                face.orientation_ = PoseData::FaceUp;
            } else {
                face.orientation_ = PoseData::FaceDown;
            }
        } else {
            face.orientation_ = PoseData::FaceUp;
        }

        if (face.orientation_ != previousFace.orientation_)
        {
            previousFace.orientation_ = face.orientation_;
            face.timestamp_ = data.timestamp_;
            faceSource.propagate(1, &face);
        }
    }
}

void OrientationInterpreter::processOrientation()
{
    PoseData newPose;

    if (topEdge.orientation_ != PoseData::Undefined)
    {
        newPose.orientation_ = topEdge.orientation_;
    } else {
        newPose.orientation_ = face.orientation_;
    }

    if (newPose.orientation_ != o_.orientation_) {
        o_.orientation_ = newPose.orientation_;
        sensordLogT() << "New orientation value:" << o_.orientation_;
        o_.timestamp_ = data.timestamp_;
        orientationSource.propagate(1, &o_);
    }
}
