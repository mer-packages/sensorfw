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
    Sink<CompassFilter, TimedXyzData> orientDataSink;

    Source<CompassData> magSource;

    void magDataAvailable(unsigned, const CalibratedMagneticFieldData*);
    void accelDataAvailable(unsigned, const AccelerationData*);
    void orientDataAvailable(unsigned, const TimedXyzData*);

    int factor;
    CalibratedMagneticFieldData magData;

    qreal magRX;
    qreal magRY;
    qreal magRZ;

    qreal adjX;
    qreal adjY;
    qreal adjZ;

    qreal level;
    qreal oldHeading;
};

#endif
