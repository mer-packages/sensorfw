/**
   @file orientationinterpreter.cpp
   @brief OrientationInterpreter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>
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

#include "orientationinterpreter.h"
#include "logging.h"
#include "config.h"
#include <math.h>
#include <stdlib.h>
#include <limits.h>

const float OrientationInterpreter::RADIANS_TO_DEGREES = 180.0/M_PI;
const int OrientationInterpreter::SAME_AXIS_LIMIT = 5;
const int OrientationInterpreter::OVERFLOW_MIN = 0;
const int OrientationInterpreter::OVERFLOW_MAX = INT_MAX;
const int OrientationInterpreter::THRESHOLD_LANDSCAPE = 25;
const int OrientationInterpreter::THRESHOLD_PORTRAIT = 20;
const int OrientationInterpreter::THRESHOLD_FACE = 900;

const int OrientationInterpreter::DISCARD_TIME = 750000;
const int OrientationInterpreter::AVG_BUFFER_MAX_SIZE = 10;
const char* OrientationInterpreter::CPU_BOOST_PATH = "/sys/power/pm_optimizer_rotation";
typedef PoseData (OrientationInterpreter::*ptrFUN)(int);

OrientationInterpreter::OrientationInterpreter() :
        accDataSink(this, &OrientationInterpreter::accDataAvailable),
        topEdge(PoseData::Undefined),
        newTopEdge(PoseData::Undefined),
        face(PoseData::Undefined),
        orientationData(PoseData::Undefined),
        cpuBoostFile(CPU_BOOST_PATH)

{
    addSink(&accDataSink, "accsink");
    addSource(&topEdgeSource, "topedge");
    addSource(&faceSource, "face");
    addSource(&orientationSource, "orientation");

    minLimit = Config::configuration()->value("orientation_overflow_min", QVariant(OVERFLOW_MIN)).toInt();
    maxLimit = Config::configuration()->value("orientation_overflow_max", QVariant(OVERFLOW_MAX)).toInt();

    angleThresholdPortrait = Config::configuration()->value("orientation_threshold_portrait", QVariant(THRESHOLD_PORTRAIT)).toInt();
    angleThresholdLandscape = Config::configuration()->value("orientation_threshold_landscape", QVariant(THRESHOLD_LANDSCAPE)).toInt();
    faceThreshold = Config::configuration()->value("face_threshold", QVariant(THRESHOLD_FACE)).toInt();


    discardTime = Config::configuration()->value("orientation_discard_time", QVariant(DISCARD_TIME)).toUInt();

    maxBufferSize = Config::configuration()->value("orientation_buffer_size", QVariant(AVG_BUFFER_MAX_SIZE)).toInt();

    // Open the handle for boosting cpu on changes that affect orientation
    if (!cpuBoostFile.exists() || !cpuBoostFile.open(QIODevice::WriteOnly))
    {
        sensordLogW() << "Failed to open" << CPU_BOOST_PATH << "for adjusting cpu freq for orientation.";
    }
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

    // Clear old values from buffer.
    while (dataBuffer.count() > maxBufferSize || (dataBuffer.count() > 1 && (data.timestamp_ - dataBuffer.first().timestamp_ > discardTime)))
    {
        dataBuffer.removeFirst();
    }

    //Calculate average
    long x = 0;
    long y = 0;
    long z = 0;
    foreach (const AccelerationData& sample, dataBuffer)
    {
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
    int vector = ((data.x_ * data.x_ + data.y_ * data.y_ + data.z_ * data.z_) / 1000);
    return !((vector >= minLimit) && (vector <= maxLimit));
}

int OrientationInterpreter::orientationCheck(const AccelerationData &data,  OrientationMode mode) const
{
    if (mode == OrientationInterpreter::Landscape)
        return round(atan((double)data.x_ / sqrt(data.y_ * data.y_ + data.z_ * data.z_)) * RADIANS_TO_DEGREES);
    else
        return round(atan((double)data.y_ / sqrt(data.x_ * data.x_ + data.z_ * data.z_)) * RADIANS_TO_DEGREES);
}

PoseData OrientationInterpreter::rotateToPortrait(int rotation)
{
    PoseData newTopEdge = PoseData::Undefined;
    newTopEdge.orientation_ = (rotation >= 0) ? PoseData::BottomUp : PoseData::BottomDown;

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
    newTopEdge.orientation_ = (rotation >= 0) ? PoseData::LeftUp : PoseData::RightUp;
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

PoseData OrientationInterpreter::orientationRotation (const AccelerationData &data, OrientationMode mode, PoseData (OrientationInterpreter::*ptrFUN)(int))
{
    int rotation = orientationCheck(data, mode);
    int threshold = (mode == OrientationInterpreter::Portrait) ? angleThresholdPortrait : angleThresholdLandscape;
    //if rotation is bigger than the threshold, then rotate using the function passed
    PoseData newTopEdge = (abs(rotation) > threshold) ? (this->*ptrFUN)(rotation) : PoseData::Undefined;
    return newTopEdge;
}

void OrientationInterpreter::processTopEdge()
{
    solveTopEdge();    

    // tmpTopEdge used to separate topEdge and orientation
    PoseData tmpTopEdge = newTopEdge;
    if (abs(data.z_)>faceThreshold){
        tmpTopEdge.orientation_ = data.z_ >= 0 ?
                                  PoseData::FaceDown : PoseData::FaceUp;
    }
    // if nothing changed
    if (topEdge.orientation_ == tmpTopEdge.orientation_) return;
    
    // Request CPU clock raise to get smooth desktop rotation
    if (cpuBoostFile.isOpen())
    {
        cpuBoostFile.write("1", 1);
        cpuBoostFile.flush();
    }

    topEdge.orientation_ = tmpTopEdge.orientation_;
    sensordLogT() << "new TopEdge value: " << topEdge.orientation_;
    topEdge.timestamp_ = data.timestamp_;
    topEdgeSource.propagate(1, &topEdge);
}

void OrientationInterpreter::processFace()
{
    if (abs(data.z_)<faceThreshold) return;

    PoseData newFace;
    newFace.orientation_ = data.z_ >= 0 ? PoseData::FaceDown : PoseData::FaceUp;
    if (face.orientation_ == newFace.orientation_) return;

    face.orientation_ = newFace.orientation_;
    face.timestamp_ = data.timestamp_;
    faceSource.propagate(1, &face);
}

void OrientationInterpreter::processOrientation()
{
    PoseData newOrientation;
    newOrientation.orientation_ = newTopEdge.orientation_== PoseData::Undefined?
                                face.orientation_:newTopEdge.orientation_;


    if (orientationData.orientation_ == newOrientation.orientation_) return;

    orientationData.orientation_ = newOrientation.orientation_;
    sensordLogT() << "New orientation value: " << orientationData.orientation_;
    orientationData.timestamp_ = data.timestamp_;
    orientationSource.propagate(1, &orientationData);
}


void OrientationInterpreter::solveTopEdge(){
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

    if (newTopEdge.orientation_ == PoseData::Undefined) //not rotated yet, then check for the other threshold
    {
        mode = (mode == OrientationInterpreter::Portrait) ? (OrientationInterpreter::Landscape) : (OrientationInterpreter::Portrait);
        rotator = (rotator == (&OrientationInterpreter::rotateToPortrait)) ? (&OrientationInterpreter::rotateToLandscape) : (&OrientationInterpreter::rotateToPortrait);
        newTopEdge = orientationRotation(data, mode, rotator);
    }
}
