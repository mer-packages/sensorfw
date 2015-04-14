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

#include <QFile>

class CalibrationFilter : public QObject, public Filter<CalibratedMagneticFieldData, CalibrationFilter, CalibratedMagneticFieldData>
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

    Sink<CalibrationFilter, CalibratedMagneticFieldData> magDataSink;

    Source<CalibratedMagneticFieldData> magSource;
    void magDataAvailable(unsigned, const CalibratedMagneticFieldData * );

    CalibratedMagneticFieldData magData;
    CalibratedMagneticFieldData transformed;

    QList <QPair<int,int> > minMaxList;

    qreal offsetX;
    qreal offsetY;
    qreal offsetZ;

    qreal xScale;
    qreal yScale;
    qreal zScale;

    qreal meanX;
    qreal meanY;
    qreal meanZ;

    qreal calLevel;
    int lowPass(int newVal, int oldVal);
    QList<const CalibratedMagneticFieldData *> *readingBuffer;
    int bufferPos;

    QFile unCalibratedData;
    QFile calibratedData;
    QTextStream stream;
    QTextStream calibratedStream;
    int dataPoints;
    bool manualCalibration;

};

#endif
