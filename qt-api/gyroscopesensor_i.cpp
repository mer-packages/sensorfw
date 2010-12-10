/**
   @file gyroscopesensor_i.cpp
   @brief Interface for GyroscopeSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>
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
#include "gyroscopesensor_i.h"
#include <datatypes/orientationdata.h>

const char* GyroscopeSensorChannelInterface::staticInterfaceName = "local.GyroscopeSensor";

QDBusAbstractInterface* GyroscopeSensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    // ToDo: see which arguments can be made explicit
    return new GyroscopeSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}

GyroscopeSensorChannelInterface::GyroscopeSensorChannelInterface(const QString &path, int sessionId)
    : AbstractSensorChannelInterface(path, GyroscopeSensorChannelInterface::staticInterfaceName, sessionId),
      frameAvailableConnected(false)
{
}

const GyroscopeSensorChannelInterface* GyroscopeSensorChannelInterface::listenInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    // ToDo: can conversion from class name to type string be automated?
    if ( !sm.registeredAndCorrectClassName( id, GyroscopeSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<const GyroscopeSensorChannelInterface*>(sm.listenInterface(id));
}

GyroscopeSensorChannelInterface* GyroscopeSensorChannelInterface::controlInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, GyroscopeSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<GyroscopeSensorChannelInterface*>(sm.controlInterface(id));
}

void GyroscopeSensorChannelInterface::dataReceived()
{
    QVector<AngularVelocityData> values;
    while (read<AngularVelocityData>(values))
    {
        if(!frameAvailableConnected || values.size() == 1)
        {
            foreach(AngularVelocityData data, values)
                emit dataAvailable(XYZ(data));
        }
        else
        {
            QVector<XYZ> realValues;
            realValues.reserve(values.size());
            foreach(AngularVelocityData data, values)
                realValues.push_back(XYZ(data));
            emit frameAvailable(realValues);
        }
        values.clear();
    }
}


void GyroscopeSensorChannelInterface::connectNotify(const char* signal)
{
    if(QLatin1String(signal) == SIGNAL(frameAvailable(QVector<XYZ>)))
        frameAvailableConnected = true;
    QDBusAbstractInterface::connectNotify(signal);
}

QDBusReply<void> GyroscopeSensorChannelInterface::reset()
{
    QList<QVariant> argumentList;
    return callWithArgumentList(QDBus::Block, QLatin1String("reset"), argumentList);
}

XYZ GyroscopeSensorChannelInterface::get() const
{
    return qvariant_cast<XYZ>(internalPropGet("value"));
}
