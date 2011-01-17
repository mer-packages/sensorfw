/**
   @file rotationsensor_i.cpp
   @brief Interface for RotationSensor

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
#include "rotationsensor_i.h"

const char* RotationSensorChannelInterface::staticInterfaceName = "local.RotationSensor";

QDBusAbstractInterface* RotationSensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    // ToDo: see which arguments can be made explicit
    return new RotationSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}

RotationSensorChannelInterface::RotationSensorChannelInterface(const QString &path, int sessionId) :
    AbstractSensorChannelInterface(path, RotationSensorChannelInterface::staticInterfaceName, sessionId),
    frameAvailableConnected(false)
{
}

const RotationSensorChannelInterface* RotationSensorChannelInterface::listenInterface(const QString& id)
{
    return dynamic_cast<const RotationSensorChannelInterface*>(interface(id));
}

RotationSensorChannelInterface* RotationSensorChannelInterface::controlInterface(const QString& id)
{
    return interface(id);
}

RotationSensorChannelInterface* RotationSensorChannelInterface::interface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, RotationSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<RotationSensorChannelInterface*>(sm.interface(id));
}

void RotationSensorChannelInterface::dataReceived()
{
    QVector<TimedXyzData> values;
    while (read<TimedXyzData>(values))
    {
        if(!frameAvailableConnected || values.size() == 1)
        {
            foreach(const TimedXyzData& data, values)
                emit dataAvailable(XYZ(data));
        }
        else
        {
            QVector<XYZ> realValues;
            realValues.reserve(values.size());
            foreach(const TimedXyzData& data, values)
                realValues.push_back(XYZ(data));
            emit frameAvailable(realValues);
        }
        values.clear();
    }
}

XYZ RotationSensorChannelInterface::rotation() const
{
    return qvariant_cast< XYZ >(internalPropGet("rotation"));
}

bool RotationSensorChannelInterface::hasZ() const
{
    return qvariant_cast< bool >(internalPropGet("hasZ"));
}

void RotationSensorChannelInterface::connectNotify(const char* signal)
{
    if(QLatin1String(signal) == SIGNAL(frameAvailable(QVector<XYZ>)))
        frameAvailableConnected = true;
    QDBusAbstractInterface::connectNotify(signal);
}
