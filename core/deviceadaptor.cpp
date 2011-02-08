/**
   @file deviceadaptor.cpp
   @brief Base class for device adaptors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#include "deviceadaptor.h"

DeviceAdaptor::~DeviceAdaptor()
{
    delete sensor_.second;
}

void DeviceAdaptor::setAdaptedSensor(const QString& name, const QString& description, RingBufferBase* buffer)
{
    setAdaptedSensor(name, new AdaptedSensorEntry(name, description, buffer));
}

void DeviceAdaptor::setAdaptedSensor(const QString& name, AdaptedSensorEntry* newAdaptedSensor)
{
    sensor_ = qMakePair(name, newAdaptedSensor);
}

AdaptedSensorEntry* DeviceAdaptor::getAdaptedSensor() const
{
    return sensor_.second;
}

RingBufferBase* DeviceAdaptor::findBuffer(const QString& name) const
{
    AdaptedSensorEntry* entry = getAdaptedSensor();
    if ( !entry )
        return NULL;
    return entry->buffer();
}

bool DeviceAdaptor::setStandbyOverride(bool override)
{
    standbyOverride_ = override;

    if (screenBlanked)
    {
        if(override)
        {
            resume();
        }
        else
        {
            standby();
        }
    }
    sensordLogD() << "standbyOverride changed: id = " << id() << ", value = " <<  standbyOverride_;
    return true;
}
