/**
   @file proximitysensor_i.cpp
   @brief Interface for ProximitySensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#include "sensormanagerinterface.h"
#include "proximitysensor_i.h"

const char* ProximitySensorChannelInterface::staticInterfaceName = "local.ProximitySensor";

AbstractSensorChannelInterface* ProximitySensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    return new ProximitySensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}


ProximitySensorChannelInterface::ProximitySensorChannelInterface(const QString& path, int sessionId)
    : AbstractSensorChannelInterface(path, ProximitySensorChannelInterface::staticInterfaceName, sessionId)
{
}

const ProximitySensorChannelInterface* ProximitySensorChannelInterface::listenInterface(const QString& id)
{
    return dynamic_cast<const ProximitySensorChannelInterface*>(interface(id));
}

ProximitySensorChannelInterface* ProximitySensorChannelInterface::controlInterface(const QString& id)
{
    return interface(id);
}

ProximitySensorChannelInterface* ProximitySensorChannelInterface::interface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, ProximitySensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }
    return dynamic_cast<ProximitySensorChannelInterface*>(sm.interface(id));
}

bool ProximitySensorChannelInterface::dataReceivedImpl()
{
    QVector<TimedUnsigned> values;
    if(!read<TimedUnsigned>(values))
        return false;
    foreach(const TimedUnsigned& data, values)
        emit dataAvailable(data);
    return true;
}

Unsigned ProximitySensorChannelInterface::proximity()
{
    return getAccessor<Unsigned>("proximity");
}
