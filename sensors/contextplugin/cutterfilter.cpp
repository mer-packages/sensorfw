/**
   @file cutterfilter.cpp
   @brief CutterFilter
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

#include "cutterfilter.h"
#include <QDebug>

CutterFilter::CutterFilter(double divider) :
    Filter<double, CutterFilter, double>(this, &CutterFilter::interpret),
    divider(divider)
{
    //qDebug() << "Creating the CutterFilter";
}

void CutterFilter::interpret(unsigned, const double* data)
{
    double n = *data / divider;
    source_.propagate(1, &n);
}
