/**
   @file orientation.cpp
   @brief QObject based datatype for OrientationData

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
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

#include "orientation.h"

Orientation::Orientation(const OrientationData& orientationData)
    : QObject(), data_(orientationData.timestamp_, orientationData.x_, orientationData.y_, orientationData.z_)
{
}

Orientation::Orientation(const Orientation& orientation)
    : QObject(), data_(orientation.orientationData().timestamp_, orientation.orientationData().x_, orientation.orientationData().y_, orientation.orientationData().z_)
{
}
