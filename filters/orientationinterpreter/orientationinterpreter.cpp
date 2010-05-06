/**
   @file orientationinterpreter.cpp
   @brief OrientationInterpreterFilter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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
#include "sensord/logging.h"


OrientationInterpreter::OrientationInterpreter() :
        Filter<TimedXyzData, OrientationInterpreter, PoseData>(this, &OrientationInterpreter::interpret),
        threshold_(*this),
        newAxis_(0),
        prevAxis_(0)
{
    qRegisterMetaType<PoseData>("PoseData");
    threshold_(DEFAULT_THRESHOLD);
}

void OrientationInterpreter::interpret(unsigned, const TimedXyzData* data)
{
    int v[3];
    v[0] = data->x_;
    v[1] = data->y_;
    v[2] = data->z_;
    sensordLogT() << "orientation vector:" << v[0] << "," << v[1] << "," << v[2];

    int gVector = ((v[0]*v[0] + v[1]*v[1] + v[2]*v[2])/1000);
    bool goodVector = ((gVector >= 800) && (gVector <=1250)) ? true : false;
    if (!goodVector)
        sensordLogT() << "vector is not good...";

    if (goodVector) {
        int max = abs(v[0]);
        newAxis_ = 0;

        if (abs(v[1]) > max) {
            newAxis_ = 1;
            max = abs(v[1]);
        }
        if (abs(v[2]) > max) {
            newAxis_ = 2;
            max = abs(v[2]);
        }

        sensordLogT() << "max vector: v[" << newAxis_ << "]" << max;

        if (pose.orientation_ == PoseData::Undefined) {
            switch (newAxis_) {
                case 0:
                    if (v[0] > 0)
                        newPose.orientation_ = PoseData::LeftUp;
                    else
                        newPose.orientation_ = PoseData::RightUp;
                    break;
                case 1:
                    if (v[1] > 0)
                        newPose.orientation_ = PoseData::BottomUp;
                    else
                        newPose.orientation_ = PoseData::BottomDown;
                    break;
                case 2:
                    if (v[2] > 0)
                        newPose.orientation_ = PoseData::FaceDown;
                    else
                        newPose.orientation_ = PoseData::FaceUp;
                    break;
            }
        } else {
            if (pose.orientation_ == PoseData::LeftUp || pose.orientation_ == PoseData::RightUp) {
                if (newAxis_ != 0) {
                    if (abs(v[newAxis_]) > 550) {
                        if (newAxis_ == 1)
                            newPose.orientation_ = (v[1] > 0?PoseData::BottomUp:PoseData::BottomDown);
                        if (newAxis_ == 2)
                            newPose.orientation_ = (v[2] > 0?PoseData::FaceDown:PoseData::FaceUp);
                    }
                } else {
                    newPose.orientation_ = (v[0] > 0?PoseData::LeftUp:PoseData::RightUp);
                }
            } else if (pose.orientation_ == PoseData::BottomDown || pose.orientation_ == PoseData::BottomUp) {
                if (newAxis_ != 1) {
                    if (abs(v[newAxis_]) > 550) {
                        if (newAxis_ == 0)
                            newPose.orientation_ = (v[0] > 0?PoseData::LeftUp:PoseData::RightUp);
                        if (newAxis_ == 2)
                            newPose.orientation_ = (v[2] > 0?PoseData::FaceDown:PoseData::FaceUp);
                    }
                } else {
                    newPose.orientation_ = (v[1] > 0?PoseData::BottomUp:PoseData::BottomDown);
                }
            }  else if (pose.orientation_ == PoseData::FaceDown || pose.orientation_ == PoseData::FaceUp) {
                if (newAxis_ != 2) {
                    if (abs(v[newAxis_]) > 550) {
                        if (newAxis_ == 0)
                            newPose.orientation_ = (v[0] > 0?PoseData::LeftUp:PoseData::RightUp);
                        if (newAxis_ == 1)
                            newPose.orientation_ = (v[1] > 0?PoseData::BottomUp:PoseData::BottomDown);
                    }
                } else {
                    newPose.orientation_ = (v[2] > 0?PoseData::FaceDown:PoseData::FaceUp);
                }
            }
        }
    }

    if (pose.orientation_ != newPose.orientation_) {
        // Axis direction changed directly or difference larger than threshold
        if ((prevAxis_ == newAxis_) || (abs(abs(v[newAxis_]) - abs(v[prevAxis_])) > threshold_())) {
            pose.orientation_ = newPose.orientation_;
            sensordLogT() << "Orientation is:" << pose.orientation_;
            prevAxis_ = newAxis_;
        }
    }
    pose.timestamp_ = data->timestamp_;
    source_.propagate(1, &pose);
}
