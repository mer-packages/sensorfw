/**
   @file rotationfilter.h
   @brief RotationFilter

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

#ifndef ROTATIONFILTER_H
#define ROTATIONFILTER_H

#include <QObject>

#include "orientationdata.h"
#include "sensord/filter.h"

#define DEFAULT_THRESHOLD 50

/**
 * @brief Filter for calculating device axis rotations.
 *
 * Axis rotations are given in degrees. Rotation is defined as the angle
 * between the acceleration vector and the positive axis.
 */
class RotationFilter : public QObject, public Filter<TimedXyzData, RotationFilter, TimedXyzData>
{
    Q_OBJECT;
public:
    /**
     * Factory method.
     * @return New RotationFilter instance as FilterBase*.
     */
    static FilterBase* factoryMethod()
    {
        return new RotationFilter();
    }

private:

    /**
     * Default constructor.
     */
    RotationFilter();

    void interpret(unsigned, const TimedXyzData*);

    inline int dotProduct(TimedXyzData a, TimedXyzData b) const {
        return (a.x_*b.x_)+(a.y_*b.y_)+(a.z_*b.z_);
    }
    
    TimedXyzData rotation_;
};

#endif // ROTATIONFILTER_H
