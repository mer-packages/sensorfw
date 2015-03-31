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
#define FILTER_COUNT 10

AvgAccFilter::AvgAccFilter() :
    Filter<TimedXyzData, AvgAccFilter, TimedXyzData>(this, &AvgAccFilter::interpret),
    avgAccdata(0,0,0,0),
    filterFactor(0.54)
{
}

void AvgAccFilter::interpret(unsigned, const TimedXyzData *data)
{
    avgAccdata.x_ = data->x_ * filterFactor + averageX * (1.0f - filterFactor);
    avgAccdata.y_ = data->y_ * filterFactor + averageY * (1.0f - filterFactor);
    avgAccdata.z_ = data->z_ * filterFactor + averageZ * (1.0f - filterFactor);

    TimedXyzData filteredData(data->timestamp_,
                              avgAccdata.x_,
                              avgAccdata.y_,
                              avgAccdata.z_);

    source_.propagate(1, &filteredData);

    averageX = data->x_;
    averageY = data->y_;
    averageZ = data->z_;
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

