/**
   @file orientationsensor_i.cpp
   @brief Interface for OrientationSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#include "sensormanagerinterface.h"
#include "orientationsensor_i.h"

const char* OrientationSensorChannelInterface::staticInterfaceName = "local.OrientationSensor";

QDBusAbstractInterface* OrientationSensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    // ToDo: see which arguments can be made explicit
    return new OrientationSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}

OrientationSensorChannelInterface::OrientationSensorChannelInterface(const QString &path, int sessionId) :
    AbstractSensorChannelInterface(path, OrientationSensorChannelInterface::staticInterfaceName, sessionId)
{
}

const OrientationSensorChannelInterface* OrientationSensorChannelInterface::listenInterface(const QString& id)
{
    return dynamic_cast<const OrientationSensorChannelInterface*>(interface(id));
}

OrientationSensorChannelInterface* OrientationSensorChannelInterface::controlInterface(const QString& id)
{
    return interface(id);
}

OrientationSensorChannelInterface* OrientationSensorChannelInterface::interface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, OrientationSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<OrientationSensorChannelInterface*>(sm.interface(id));
}

void OrientationSensorChannelInterface::dataReceived()
{
    QVector<TimedUnsigned> values;
    while (read<TimedUnsigned>(values))
    {
        foreach(TimedUnsigned data, values)
            emit orientationChanged(data);
        values.clear();
    }
}

Unsigned OrientationSensorChannelInterface::orientation() const
{
    return qvariant_cast<Unsigned>(internalPropGet("orientation"));
}

int OrientationSensorChannelInterface::threshold() const
{
    return qvariant_cast<int>(internalPropGet("threshold"));
}

void OrientationSensorChannelInterface::setThreshold(int value)
{
    internalPropSet("threshold", qVariantFromValue(value));
}
