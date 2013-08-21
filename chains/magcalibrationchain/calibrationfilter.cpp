/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com

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
 */

#include <QDebug>

#include "calibrationfilter.h"
#include "config.h"

CalibrationFilter::CalibrationFilter() :
    Filter<TimedXyzData, CalibrationFilter, CalibratedMagneticFieldData>(this, &CalibrationFilter::magDataAvailable),
    magDataSink(this, &CalibrationFilter::magDataAvailable),
    calLevel(0)
{
    addSink(&magDataSink, "magsink");
    addSource(&magSource, "calibratedmagneticfield");
// min, max
    minMaxList.insert(0,qMakePair(0,0));
    minMaxList.insert(1,qMakePair(0,0));
    minMaxList.insert(2,qMakePair(0,0));
}

#define LISTCOUNT 10

void CalibrationFilter::magDataAvailable(unsigned, const TimedXyzData *data)
{
    CalibratedMagneticFieldData transformed;

    transformed.timestamp_ = data->timestamp_;

  //  if (calLevel != 3) {
        //    simple hard iron correction
        if (minMaxList.at(0).first == 0) {
            minMaxList.replace(0,qMakePair(data->x_, data->x_));
            minMaxList.replace(1,qMakePair(data->y_, data->y_));
            minMaxList.replace(2,qMakePair(data->z_, data->z_));

        } else {
            minMaxList.replace(0,qMakePair(qMin(minMaxList.at(0).first, data->x_),
                                           qMax(minMaxList.at(0).second, data->x_)));
            minMaxList.replace(1,qMakePair(qMin(minMaxList.at(1).first, data->y_),
                                           qMax(minMaxList.at(1).second, data->y_)));
            minMaxList.replace(2,qMakePair(qMin(minMaxList.at(2).first, data->z_),
                                           qMax(minMaxList.at(2).second, data->z_)));
        }
        qreal newX = (minMaxList.at(0).first + minMaxList.at(0).second) * .5;
        qreal newY = (minMaxList.at(1).first + minMaxList.at(1).second) * .5;
        qreal newZ = (minMaxList.at(2).first + minMaxList.at(2).second) * .5;

        calLevel = 0;
        if (oldX == newX)
            calLevel += 1;
        if (oldY == newY)
            calLevel += 1;
        if (oldZ == newZ)
            calLevel += 1;

        oldX = newX;
        oldY = newY;
        oldZ = newZ;
  //  }
    transformed.level_ = calLevel;

    transformed.x_ = oldX;
    transformed.y_ = oldY;
    transformed.z_ = oldZ;

    transformed.rx_ = data->x_;
    transformed.ry_ = data->y_;
    transformed.rz_ = data->z_;

    magSource.propagate(1, &transformed);
    source_.propagate(1, &transformed);
}

void CalibrationFilter::dropCalibration()
{
    calLevel = 0;
    minMaxList.clear();
}
