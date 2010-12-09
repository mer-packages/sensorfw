/**
   @file alssensor_i.cpp
   @brief Interface for ALSSensor

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
#include "alssensor_i.h"
#include "socketreader.h"

const char* ALSSensorChannelInterface::staticInterfaceName = "local.ALSSensor";

QDBusAbstractInterface* ALSSensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    // ToDo: see which arguments can be made explicit
    return new ALSSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}

ALSSensorChannelInterface::ALSSensorChannelInterface(const QString& path, int sessionId)
    : AbstractSensorChannelInterface(path, ALSSensorChannelInterface::staticInterfaceName, sessionId)
{
}

const ALSSensorChannelInterface* ALSSensorChannelInterface::listenInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    // ToDo: can conversion from class name to type string be automated?
    if ( !sm.registeredAndCorrectClassName( id, ALSSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<const ALSSensorChannelInterface*>(sm.listenInterface(id));
}

ALSSensorChannelInterface* ALSSensorChannelInterface::controlInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, ALSSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<ALSSensorChannelInterface*>(sm.controlInterface(id));
}

void ALSSensorChannelInterface::dataReceived()
{
    QVector<TimedUnsigned> values;
    while (read<TimedUnsigned>(values))
    {
        foreach(TimedUnsigned data, values)
            emit ALSChanged(data);
        values.clear();
    }
}

Unsigned ALSSensorChannelInterface::lux() const
{
    return qvariant_cast< Unsigned >(internalPropGet("lux"));
}
