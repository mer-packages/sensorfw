/**
   @file gyroscopesensor_a.cpp
   @brief D-Bus adaptor for GyroscopeSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>

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

#include "gyroscopesensor_a.h"

GyroscopeSensorChannelAdaptor::GyroscopeSensorChannelAdaptor(QObject* parent) :
    AbstractSensorChannelAdaptor(parent)
{
    setAutoRelaySignals(true);
}

AngularVelocityData GyroscopeSensorChannelAdaptor::get() const
{
    return qvariant_cast<AngularVelocityData>(parent()->property("value"));
}
