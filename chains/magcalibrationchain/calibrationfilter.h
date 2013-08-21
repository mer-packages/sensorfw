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

#ifndef MAGCALIBRATIONFILTER_H
#define MAGCALIBRATIONFILTER_H

#include <QObject>

#include "orientationdata.h"
#include "filter.h"


class CalibrationFilter : public QObject, public Filter<TimedXyzData, CalibrationFilter, CalibratedMagneticFieldData>
{
    Q_OBJECT

public:

    static FilterBase* factoryMethod() {
        return new CalibrationFilter;
    }
    void dropCalibration();

protected:

    CalibrationFilter();

private:

    Sink<CalibrationFilter, TimedXyzData> magDataSink;

    Source<CalibratedMagneticFieldData> magSource;

    void magDataAvailable(unsigned, const TimedXyzData * );

    CalibratedMagneticFieldData magData;
    QList <QPair<int,int> > minMaxList;

    qreal oldX;
    qreal oldY;
    qreal oldZ;

    qreal calLevel;
};

#endif
