/**
   @file screeninterpreterfilter.cpp
   @brief ScreenInterpreterFilter
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>

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

#include "screeninterpreterfilter.h"
#include "genericdata.h"
#include "math.h"

#include <ContextProvider>

#include <QDebug>

ScreenInterpreterFilter::ScreenInterpreterFilter(
    ContextProvider::Property* topEdgeProperty,
    ContextProvider::Property* isCoveredProperty) :
    Filter<TimedXyzData, ScreenInterpreterFilter, TimedXyzData>(this, &ScreenInterpreterFilter::interpret),
    topEdgeProperty(topEdgeProperty),
    isCoveredProperty(isCoveredProperty),
    threshold(0.707),       // 1/sqrt(2) i.e. "half-way"
    hysteresis(0.1),        // 10%
    averageAcceleration(1000) // Earth, for starters
{
    //qDebug() << "Creating the ScreenInterpreterFilter";
}

void ScreenInterpreterFilter::interpret(unsigned, const TimedXyzData* data)
{
    //qDebug() << "Data received on ScreenInterpreter... analyzing and pushing forward";
    provideScreenData(data->x_, data->y_, data->z_);
    source_.propagate(1, data);
}

void ScreenInterpreterFilter::provideScreenData(int x, int y, int z)
{
    // Default values: TopEdge = "keep the previous one",
    // isCovered = false. They won't be Unknown (whenever data is
    // available).
    QString topEdge = "";
    bool isCovered = false;

    // FIXME: primitive avg. acceleration calculation. Need to figure out
    // how to get refresh rate.
    averageAcceleration = (averageAcceleration + sqrt(x*x + y*y + z*z)) / 2.0;

    //qDebug() << z;

    if (z > averageAcceleration * threshold * (1 + hysteresis)) {
        isCovered = false;
    } else if (abs(z) < averageAcceleration * threshold * (1 - hysteresis))  {
        isCovered = false;
        if (abs(y) > abs(x) * (1 + hysteresis))
            topEdge =  (y > 0 ? "top" : "bottom");
        else if (abs(x) > abs(y) * (1 + hysteresis)) {
            topEdge = (x > 0 ? "right" : "left");
        }
    } else if (z < -1 * averageAcceleration * threshold * (1 + hysteresis)) {
        isCovered = true;
    }

    // Provide
    if (topEdge != "")
        topEdgeProperty->setValue(topEdge);
    isCoveredProperty->setValue(isCovered);

}
