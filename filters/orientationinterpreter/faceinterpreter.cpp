/**
   @file faceinterpreter.cpp
   @brief FaceInterpreterFilter

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
#include "faceinterpreter.h"
#include "sensord/logging.h"


FaceInterpreter::FaceInterpreter() :
        Filter<TimedXyzData, FaceInterpreter, PoseData>(this, &FaceInterpreter::interpret),
        threshold_(*this),
        pose(PoseData::Undefined)
{
    qRegisterMetaType<PoseData>("PoseData");
    threshold_(DEFAULT_THRESHOLD);
}

void FaceInterpreter::interpret(unsigned, const TimedXyzData* data)
{
    int x = data->x_;
    int y = data->y_;
    int z = data->z_;
    sensordLogT() << "orientation vector:" << x << "," << y << "," << z;

    //if (pose.orientation_ == PoseData::FaceUp) {
        //if ((z > 600) &&
            //(abs(x) < 200) &&
            //(abs(y) < 200)) {
            //newPose.orientation_ = PoseData::FaceDown;
        //}
    //} else if (z < -120) {
        //newPose.orientation_ = PoseData::FaceUp;
    //}

    if  (abs(z) >= 300)
        newPose.orientation_ = ((z>=0)? PoseData::FaceDown : PoseData::FaceUp);

    if (pose.orientation_ != newPose.orientation_) {
        // Axis direction changed directly or difference larger than threshold
        pose.orientation_ = newPose.orientation_;
        sensordLogT() << "Face is:" << pose.orientation_;
        pose.timestamp_ = data->timestamp_;
        source_.propagate(1, &pose);
    }
}
