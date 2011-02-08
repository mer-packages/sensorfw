/**
   @file screeninterpreterfilter.h
   @brief ScreenInterpreterFilter
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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

#ifndef SCREENINTERPRETERFILTER_H
#define SCREENINTERPRETERFILTER_H

#include "filter.h"
#include "posedata.h"

#include <ContextProvider>

/*!

    \class ScreenInterpreterFilter

    \brief Filter for providing the Screen.TopEdge, Position.IsFlat, and
    Screen.IsCovered context properties.

    ScreenInterpreterFilter computes the context properties from
    TimedXyzData. It pushes the data forward unchanged.

*/

class ScreenInterpreterFilter : public QObject, public Filter<PoseData, ScreenInterpreterFilter, PoseData>
{
    Q_OBJECT

public:
    ScreenInterpreterFilter(ContextProvider::Property* topEdgeProperty, ContextProvider::Property* isCoveredProperty, ContextProvider::Property* isFlatProperty);

private:
    ContextProvider::Property* topEdgeProperty;
    ContextProvider::Property* isCoveredProperty;
    ContextProvider::Property* isFlatProperty;
    void interpret(unsigned, const PoseData* data);
    void provideScreenData(PoseData::Orientation orientation);

    const float threshold;
    bool isCovered;
    bool isFlat;
    PoseData::Orientation lastOrientation;
    QString topEdge;
    int offset;
    static const char* orientationValues[4];
};

#endif
