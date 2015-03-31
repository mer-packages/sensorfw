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

#ifndef COMPASSFILTER_H
#define COMPASSFILTER_H

#include <QObject>
#include "ringbuffer.h"
#include "orientationdata.h"
#include "filter.h"

class CompassFilter : public QObject, public FilterBase
{
    Q_OBJECT

public:

    static FilterBase* factoryMethod() {
        return new CompassFilter;
    }

protected:

    CompassFilter();

private:

    Sink<CompassFilter, CalibratedMagneticFieldData> magDataSink;
    Sink<CompassFilter, AccelerationData> accelSink;
    Source<CompassData> magSource;

    void magDataAvailable(unsigned, const CalibratedMagneticFieldData*);
    void accelDataAvailable(unsigned, const AccelerationData*);

    CalibratedMagneticFieldData magData;

    qreal magX;
    qreal magY;
    qreal magZ;
    qreal oldMagX;
    qreal oldMagY;
    qreal oldMagZ;

    int level;
    int oldHeading;
    QList <int> averagingBuffer;
    QList <const CalibratedMagneticFieldData *> magAvgBuffer;
    QList <const AccelerationData *> accelAvgBuffer;
//    MagAvgBuffer magAvgBuffer;
};

#endif
