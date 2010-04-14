/**
   @file raworientation.cpp
   @brief QObject based datatype for raw orientation

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>

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

#include "raworientation.h"

RawOrientation::RawOrientation(const DeviceAxesData& deviceAxesData)
    : QObject(), data_(deviceAxesData.x, deviceAxesData.y, deviceAxesData.z)
{
}

RawOrientation::RawOrientation(const RawOrientation& orientation)
    : QObject(), data_(orientation.deviceAxesData().x, orientation.deviceAxesData().y, orientation.deviceAxesData().z)
{
}
