/**
   @file orientationinterpreter.cpp
   @brief OrientationInterpreter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
   

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
#include "config.h"
#include <QDebug>

#define DEFAULT_THRESHOLD 250
#define RADIANS_TO_DEGREES 180.0/M_PI
#define ANGLE_LIMIT 45
#define SAME_AXIS_LIMIT 5

#define OVERFLOW_MIN 800
#define OVERFLOW_MAX 1250

#define THRESHOLD_LANDSCAPE  25
#define THRESHOLD_PORTRAIT  20

OrientationInterpreter::OrientationInterpreter() :
        accDataSink(this, &OrientationInterpreter::accDataAvailable),
        threshold_(*this),
        topEdge(PoseData::Undefined),
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
   
      
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(verifyFaceChange())); 
    updatePreviousFace = true; 

}

void OrientationInterpreter::verifyFaceChange()
{  
    
    if (face.orientation_ == previousFace.orientation_)
    {
        sensordLogT() << "New face value:" << face.orientation_;          
        faceSource.propagate(1, &face);
        updatePreviousFace = true;
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
    return !((gVector >= minlimit) && (gVector <=maxlimit));
}

void OrientationInterpreter::processTopEdge()
{
    // Provide something on the first run (topEdge + Face)

    int rotation;
    bool orientationTriggered = false;
    PoseData newTopEdge = PoseData::Undefined;

    // Portrait check
    rotation = round(atan((double)data.x_ / sqrt(data.y_*data.y_ + data.z_*data.z_)) * RADIANS_TO_DEGREES);

    if (abs(rotation) > angleThresholdPortrait) {
        newTopEdge.orientation_ = (rotation>=0) ? PoseData::LeftUp : PoseData::RightUp;

        // Some threshold to switching between portrait modes
        if (topEdge.orientation_ == PoseData::LeftUp || topEdge.orientation_ == PoseData::RightUp) {
            //if (topEdge.orientation_ != newTopEdge.orientation_ && rotation < SAME_AXIS_LIMIT) {
            if (abs(rotation) < SAME_AXIS_LIMIT){
                newTopEdge.orientation_ = topEdge.orientation_;
            }
        }

    } else {
        // Landscape check
        rotation = round(atan((double)data.y_ / sqrt(data.x_*data.x_ + data.z_*data.z_)) * RADIANS_TO_DEGREES);
        if (abs(rotation) > angleThresholdLandscape) {
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
}

void OrientationInterpreter::processFace()
{
    PoseData newFace;


    if  (abs(data.z_) >= 300)
    {
        newFace.orientation_ = ((data.z_>=0)? PoseData::FaceDown : PoseData::FaceUp);   

        if (face.orientation_ != newFace.orientation_)
        { 
            if (updatePreviousFace)
            {    
                face.timestamp_ = data.timestamp_;
                previousFace.orientation_ = newFace.orientation_;
                updatePreviousFace = false;
            }
            face.orientation_ = newFace.orientation_;  
            timer->start(500);
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
