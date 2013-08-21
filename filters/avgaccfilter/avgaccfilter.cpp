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

#include "avgaccfilter.h"
#include <math.h>

AvgAccFilter::AvgAccFilter() :
    Filter<TimedXyzData, AvgAccFilter, TimedXyzData>(this, &AvgAccFilter::interpret),
    avgAccdata(0,0,0,0),
    filterFactor(0.2)
{
}

void AvgAccFilter::interpret(unsigned, const TimedXyzData *data)
{
    avgAccdata.x_ = data->x_ * filterFactor + avgAccdata.x_ * (1.0 - filterFactor);
    avgAccdata.y_ = data->y_ * filterFactor + avgAccdata.y_ * (1.0 - filterFactor);
    avgAccdata.z_ = data->z_ * filterFactor + avgAccdata.z_ * (1.0 - filterFactor);

    TimedXyzData filteredData(data->timestamp_,
                              avgAccdata.x_,
                              avgAccdata.y_,
                              avgAccdata.z_);

    sensordLogT() << "averaged: "
                  << filteredData.x_
                  << ", "
                  << filteredData.y_
                  << ", " << filteredData.z_;

    source_.propagate(1, &filteredData);
}

void AvgAccFilter::reset()
{
    avgAccdata.x_ = 0;
    avgAccdata.y_ = 0;
    avgAccdata.z_ = 0;
}

void AvgAccFilter::setFactor(qreal f)
{
    filterFactor = f;
}


qreal AvgAccFilter::factor()
{
    return filterFactor;
}

