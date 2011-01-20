/**
   @file accelerometersensor_i.cpp
   @brief Interface for AccelerometerSensor

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
#include "accelerometersensor_i.h"

const char* AccelerometerSensorChannelInterface::staticInterfaceName = "local.AccelerometerSensor";

QDBusAbstractInterface* AccelerometerSensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    // ToDo: see which arguments can be made explicit
    return new AccelerometerSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}

AccelerometerSensorChannelInterface::AccelerometerSensorChannelInterface(const QString &path, int sessionId) :
    AbstractSensorChannelInterface(path, AccelerometerSensorChannelInterface::staticInterfaceName, sessionId),
    frameAvailableConnected(false)
{
}

const AccelerometerSensorChannelInterface* AccelerometerSensorChannelInterface::listenInterface(const QString& id)
{
    return dynamic_cast<const AccelerometerSensorChannelInterface*> (interface(id));
}

AccelerometerSensorChannelInterface* AccelerometerSensorChannelInterface::controlInterface(const QString& id)
{
   return interface(id);
}

AccelerometerSensorChannelInterface* AccelerometerSensorChannelInterface::interface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    // ToDo: can conversion from class name to type string be automated?
    if ( !sm.registeredAndCorrectClassName( id, AccelerometerSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }
    return dynamic_cast<AccelerometerSensorChannelInterface*>(sm.interface(id));
}

void AccelerometerSensorChannelInterface::dataReceived()
{
    QVector<AccelerationData> values;
    read<AccelerationData>(values);
    if(!frameAvailableConnected || values.size() == 1)
    {
        foreach(const AccelerationData& data, values)
            emit dataAvailable(XYZ(data));
    }
    else
    {
        QVector<XYZ> realValues;
        realValues.reserve(values.size());
        foreach(const AccelerationData& data, values)
            realValues.push_back(XYZ(data));
        emit frameAvailable(realValues);
    }
}

XYZ AccelerometerSensorChannelInterface::get() const
{
    return qvariant_cast<XYZ>(internalPropGet("value"));
}

void AccelerometerSensorChannelInterface::connectNotify(const char* signal)
{
    if(QLatin1String(signal) == SIGNAL(frameAvailable(QVector<XYZ>)))
        frameAvailableConnected = true;
    QDBusAbstractInterface::connectNotify(signal);
}
