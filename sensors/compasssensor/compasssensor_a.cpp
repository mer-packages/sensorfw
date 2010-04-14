/**
   @file compasssensor_a.cpp
   @brief D-Bus adaptor for CompassSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#include "compasssensor_a.h"

CompassSensorChannelAdaptor::CompassSensorChannelAdaptor(QObject* parent) :
    AbstractSensorChannelAdaptor(parent)
{
    setAutoRelaySignals(true);
}

Compass CompassSensorChannelAdaptor::get() const
{
    return qvariant_cast< Compass >(parent()->property("value"));
}

bool CompassSensorChannelAdaptor::useDeclination() const
{
    return qvariant_cast< bool >(parent()->property("usedeclination"));
}

void CompassSensorChannelAdaptor::setUseDeclination(bool enable)
{
    parent()->setProperty("usedeclination", enable);
}

int CompassSensorChannelAdaptor::declinationValue() const
{
    return qvariant_cast< int >(parent()->property("declinationvalue"));
}

