/**
   @file accelerometersensor_i.cpp
   @brief Interface for AccelerometerSensor

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
#include "accelerometersensor_i.h"
#include <datatypes/orientationdata.h>

AccelerometerSensorChannelInterface::AccelerometerSensorChannelInterface(const QString &path, int sessionId)
    : AbstractSensorChannelInterface(path, AccelerometerSensorChannelInterface::staticInterfaceName(), sessionId)
{
}

const AccelerometerSensorChannelInterface* AccelerometerSensorChannelInterface::listenInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    // ToDo: can conversion from class name to type string be automated?
    if ( !sm.registeredAndCorrectClassName( id, AccelerometerSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<const AccelerometerSensorChannelInterface*>(sm.listenInterface(id));
}

AccelerometerSensorChannelInterface* AccelerometerSensorChannelInterface::controlInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, AccelerometerSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<AccelerometerSensorChannelInterface*>(sm.controlInterface(id));
}

void AccelerometerSensorChannelInterface::dataReceived()
{
    AccelerationData value;
    while (socketReader_->read((void *)&value, sizeof(AccelerationData))) {
        emit dataAvailable(XYZ(value));
    }
}
