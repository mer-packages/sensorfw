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
    Filter<PoseData, ScreenInterpreterFilter, PoseData>(this, &ScreenInterpreterFilter::interpret),
    topEdgeProperty(topEdgeProperty),
    isCoveredProperty(isCoveredProperty),
    threshold(230),
    isCovered(false)
{
    //qDebug() << "Creating the ScreenInterpreterFilter";
}

void ScreenInterpreterFilter::interpret(unsigned, const PoseData* data)
{
    sensordLogT() << "Data received on ScreenInterpreter... " << data->timestamp_;
    provideScreenData(data->orientation_);
    source_.propagate(1, data);
}

void ScreenInterpreterFilter::provideScreenData(PoseData::Orientation orientation)
{
    // Provide
    QString topEdge;

    if (orientation != PoseData::Undefined) {
        sensordLogT() << "Screen orientation:" << orientation;

        switch (orientation) {
            case PoseData::FaceUp:
                isCovered = false;
                break;
            case PoseData::FaceDown:
                isCovered = true;
                break;
            case PoseData::LeftUp:
                topEdge = "left";
                break;
            case PoseData::RightUp:
                topEdge = "right";
                break;
            case PoseData::BottomDown:
                topEdge = "top";
                break;
            case PoseData::BottomUp:
                topEdge = "bottom";
                break;
        }

        topEdgeProperty->setValue(topEdge);
    }
    isCoveredProperty->setValue(isCovered);

}
