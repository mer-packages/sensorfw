/**
   @file rotationsensor_i.cpp
   @brief Interface for RotationSensor

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

#include "sensormanagerinterface.h"
#include "rotationsensor_i.h"

RotationSensorChannelInterface::RotationSensorChannelInterface(const QString &path, int sessionId)
    : AbstractSensorChannelInterface(path, RotationSensorChannelInterface::staticInterfaceName(), sessionId) {}

const RotationSensorChannelInterface* RotationSensorChannelInterface::listenInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    // ToDo: can conversion from class name to type string be automated?
    if ( !sm.registeredAndCorrectClassName( id, RotationSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<const RotationSensorChannelInterface*>(sm.listenInterface(id));
}

RotationSensorChannelInterface* RotationSensorChannelInterface::controlInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, RotationSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<RotationSensorChannelInterface*>(sm.controlInterface(id));
}

void RotationSensorChannelInterface::dataReceived()
{
    TimedXyzData value;
    while (socketReader_->read((void *)&value, sizeof(TimedXyzData))) {
        emit dataAvailable(XYZ(value));
    }
}
