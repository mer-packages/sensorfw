/**
   @file faceinterpreter.h
   @brief FaceInterpreter

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

#ifndef FACEINTERPRETER_H
#define FACEINTERPRETER_H

#include <QObject>
#include "posedata.h"
#include "datatypes/unsigned.h"
#include "orientationdata.h"
#include "sensord/filter.h"
#include "sensord/filterproperty.h"

#define DEFAULT_THRESHOLD 50

/**
 * @brief Filter for calculating the face orientation.
 *
 * This filter reports if the device screen is facing upwards or downwards.
 *
 */
class FaceInterpreter : public QObject, public Filter<TimedXyzData, FaceInterpreter, PoseData>, public PropertyTracker
{
    Q_OBJECT;

    /**
     * Holds the threshold value for the orientation calculation.
     */
    Q_PROPERTY(int threshold READ threshold_ WRITE threshold_);
    Q_PROPERTY(PoseData face READ face);

public:
    /**
     * Factory method.
     * @return New FaceInterpreter instance as FilterBase*.
     */
    static FilterBase* factoryMethod()
    {
        return new FaceInterpreter();
    }

    /**
     * Property method for getting the current orientation
     * @returns Current Face
     */
    PoseData face() const
    {
        return pose;
    }


private:
    FilterProperty<int> threshold_;

    /**
     * Default constructor.
     */
    FaceInterpreter();

    void interpret(unsigned, const TimedXyzData*);
    PoseData pose;
    PoseData newPose;
};

#endif // FACEINTERPRETER_H
