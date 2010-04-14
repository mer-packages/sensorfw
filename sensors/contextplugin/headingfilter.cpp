/**
   @file headingfilter.cpp
   @brief HeadingFilter
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

#include "headingfilter.h"
#include <QDebug>

HeadingFilter::HeadingFilter(Property* headingProperty) :
    Filter<CompassData, HeadingFilter, CompassData>(this, &HeadingFilter::interpret),
    headingProperty(headingProperty)
{
}

void HeadingFilter::interpret(unsigned, const CompassData* data)
{
    headingProperty->setValue(data->degrees_);
    source_.propagate(1, data);
}

