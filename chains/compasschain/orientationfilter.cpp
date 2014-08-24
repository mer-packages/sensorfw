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
#include "orientationfilter.h"
#include "config.h"


OrientationFilter::OrientationFilter()
    : orientDataSink(this, &OrientationFilter::orientDataAvailable),
      level(0) //presume this is fully calibrated
{
    addSink(&orientDataSink, "orientsink");
    addSource(&magSource, "magnorthangle");
}

void OrientationFilter::orientDataAvailable(unsigned, const CompassData *data)
{
    CompassData compassData; //north angle
    compassData.timestamp_ = data->timestamp_;
    compassData.degrees_ = data->degrees_ * .001;
    if (data->level_ > 0 && data->level_ < 3.1)
        compassData.level_ = data->level_;
    else
        compassData.level_ = level;
    magSource.propagate(1, &compassData);
}
