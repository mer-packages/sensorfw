/**
   @file screeninterpreterfilter.cpp
   @brief ScreenInterpreterFilter
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>
   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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
#include "logging.h"

#include <ContextProvider>

#include <QDebug>

ScreenInterpreterFilter::ScreenInterpreterFilter(
    ContextProvider::Property* topEdgeProperty,
    ContextProvider::Property* isCoveredProperty) :
    Filter<TimedXyzData, ScreenInterpreterFilter, TimedXyzData>(this, &ScreenInterpreterFilter::interpret),
    topEdgeProperty(topEdgeProperty),
    isCoveredProperty(isCoveredProperty),
    threshold(230)
{
    //qDebug() << "Creating the ScreenInterpreterFilter";
}

void ScreenInterpreterFilter::interpret(unsigned, const TimedXyzData* data)
{
    sensordLogT() << "Data received on ScreenInterpreter... " << data->timestamp_;
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

    /* Device orientation calculations. Mostly copied from Fremantle
     * MCE code.*/
    int gVector = ((x*x + y*y + z*z)/1000);
    bool goodVector = ((gVector >= 800) && (gVector <=1250)) ? true : false;
    if (!goodVector)
        qDebug() << "vector is not good...";

    /* Face up or down? */
    if (isCovered == false) {
        if ( (z > 800) && (x < 200) && (y < 200)) {
            isCovered = true;
        }
    } else if ( z < -120 ) {
        isCovered = false;
    }

    if (!goodVector) {
        topEdge = "";
    } else if ((abs(x) >= 200) || (abs(y) >= 200)) {
        if (topEdge == "") {
            /*change topEdge away from unknown
             * when x or y goes above 400 */
            if ( (abs(y) > 400) && (abs(y) > abs(x)) ) {
                if (y < 0)
                    topEdge = "bottom";
                else
                    topEdge = "top";
            } else if ( (abs(x) > 400) && (abs(x) > abs(y)) ) {
                if (x < 0)
                    topEdge = "left";
                else
                    topEdge = "right";
            }
        } else if (topEdge == "top" || topEdge == "bottom") {
            /* change topEdge when x is bigger than y as much as the threshold.*/
            if ( abs(x) > (abs(y) + threshold) ) {
                if ( x < 0 )
                    topEdge = "left";
                else
                    topEdge = "right";
            } else {
                if ( y < 0 )
                    topEdge = "top";
                else
                    topEdge = "bottom";
            }
        } else if (topEdge == "left" || topEdge == "right") {
            /* change topEdge when y is bigger than x as much as the threshold.*/
            if (abs(y) > (abs(x) + threshold)) {
                if (y < 0)
                    topEdge = "top";
                else
                    topEdge = "bottom";
            } else {
                if (x < 0)
                    topEdge = "left";
                else
                    topEdge = "right";
            }
        }
    } else {
        topEdge = "";
    }

    // Provide
    if (topEdge != "") {
        sensordLogT() << "Screen orientation:" << topEdge;
        topEdgeProperty->setValue(topEdge);
    }
    isCoveredProperty->setValue(isCovered);

}
