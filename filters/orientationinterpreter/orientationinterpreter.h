/**
   @file orientationinterpreter.h
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

#ifndef ORIENTATIONINTERPRETER_H
#define ORIENTATIONINTERPRETER_H

#include <QObject>
#include "posedata.h"
#include "datatypes/unsigned.h"
#include "orientationdata.h"
#include "sensord/filter.h"
#include "sensord/filterproperty.h"

/**
 * @brief Filter for calculating the device orientation.
 *
 * Filter for calculating the device orientation. The orientation is given in
 * 2 axis and 2 directions per axis.
 *
 * Orientation of the face is handled in the #FaceInterpreter filter.
 */
class OrientationInterpreter : public QObject, public Filter<TimedXyzData, OrientationInterpreter, PoseData>, public PropertyTracker
{
    Q_OBJECT;

    /**
     * Holds the threshold value for the orientation calculation.
     */
    Q_PROPERTY(int threshold READ threshold_ WRITE threshold_);
    Q_PROPERTY(PoseData orientation READ orientation);

public:
    /**
     * Factory method.
     * @return New OrientationInterpreter instance as FilterBase*.
     */
    static FilterBase* factoryMethod()
    {
        return new OrientationInterpreter();
    }

    /**
     * Property method for getting the current orientation
     * @returns Current Orientation
     */
    PoseData orientation() const
    {
        return pose;
    }


private:
    FilterProperty<int> threshold_;

    /**
     * Default constructor.
     */
    OrientationInterpreter();

    void interpret(unsigned, const TimedXyzData*);
    PoseData pose;
    PoseData newPose;
};

#endif // ORIENTATIONINTERPRETER_H
