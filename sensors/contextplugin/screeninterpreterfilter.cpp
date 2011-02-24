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
#include "config.h"
#include "logging.h"

#include <ContextProvider>

const char* ScreenInterpreterFilter::orientationValues[4] = {"left", "top", "right", "bottom"};

ScreenInterpreterFilter::ScreenInterpreterFilter(
    ContextProvider::Property* topEdgeProperty,
    ContextProvider::Property* isCoveredProperty,
    ContextProvider::Property* isFlatProperty) :
    Filter<PoseData, ScreenInterpreterFilter, PoseData>(this, &ScreenInterpreterFilter::interpret),
    topEdgeProperty(topEdgeProperty),
    isCoveredProperty(isCoveredProperty),
    isFlatProperty(isFlatProperty),
    threshold(230),
    isCovered(false),
    isFlat(false),
    lastOrientation(PoseData::BottomDown),
    topEdge("left")
{
    // Get offset from config
    offset = Config::configuration()->value("orientation_offset", QVariant(0)).toInt();
}

void ScreenInterpreterFilter::interpret(unsigned, const PoseData* data)
{
    sensordLogT() << "Data received on ScreenInterpreter... " << data->timestamp_;
    provideScreenData(data->orientation_);
    source_.propagate(1, data);
}

void ScreenInterpreterFilter::provideScreenData(PoseData::Orientation orientation)
{

    sensordLogT() << "Screen orientation from contextprovider:" << orientation;

    // Any TopEdge value should set flat to false.
    if (isFlat && orientation != PoseData::Undefined && orientation != PoseData::FaceDown && orientation != PoseData::FaceUp)
    {
        isFlat = false;
    }

    switch (orientation) {
        case PoseData::FaceUp:
            isCovered = false;
            break;
        case PoseData::FaceDown:
            isCovered = true;
            break;
        case PoseData::LeftUp:
            topEdge = "left";
            topEdge = orientationValues[(0+offset)%4];
            break;
        case PoseData::RightUp:
            topEdge = "right";
            topEdge = orientationValues[(2+offset)%4];
            break;
        case PoseData::BottomDown:
            topEdge = "top";
            topEdge = orientationValues[(1+offset)%4];
            break;
        case PoseData::BottomUp:
            topEdge = "bottom";
            topEdge = orientationValues[(3+offset)%4];
            break;
        case PoseData::Undefined:
            isFlat = true;
            break;
        default:
            topEdge = "top";
            break;
    }

    topEdgeProperty->setValue(topEdge);
    isCoveredProperty->setValue(isCovered);
    isFlatProperty->setValue(isFlat);
}
