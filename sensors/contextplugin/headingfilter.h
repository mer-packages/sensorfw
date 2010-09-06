/**
   @file headingfilter.h
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

#ifndef HEADINGFILTER_H
#define HEADINGFILTER_H

#include "sensord/filter.h"
#include "datatypes/orientationdata.h"

#include <ContextProvider>

using ContextProvider::Property;

class HeadingFilter : public QObject, public Filter<CompassData, HeadingFilter, CompassData>
{
    Q_OBJECT

public:
    HeadingFilter(Property* headingProperty);
    void reset();

private:
    Property* headingProperty;
    void interpret(unsigned, const CompassData* data);
};

#endif
