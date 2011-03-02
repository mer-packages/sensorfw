/**
   @file magnetometersensor_i.cpp
   @brief Interface for MagnetometerSensor

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
#include "magnetometersensor_i.h"

const char* MagnetometerSensorChannelInterface::staticInterfaceName = "local.MagnetometerSensor";

AbstractSensorChannelInterface* MagnetometerSensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    return new MagnetometerSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}

MagnetometerSensorChannelInterface::MagnetometerSensorChannelInterface(const QString& path, int sessionId) :
    AbstractSensorChannelInterface(path, MagnetometerSensorChannelInterface::staticInterfaceName, sessionId),
    frameAvailableConnected(false)
{
}

const MagnetometerSensorChannelInterface* MagnetometerSensorChannelInterface::listenInterface(const QString& id)
{
    return dynamic_cast<const MagnetometerSensorChannelInterface*>(interface(id));
}

MagnetometerSensorChannelInterface* MagnetometerSensorChannelInterface::controlInterface(const QString& id)
{
    return interface(id);
}

MagnetometerSensorChannelInterface* MagnetometerSensorChannelInterface::interface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, MagnetometerSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<MagnetometerSensorChannelInterface*>(sm.interface(id));
}

bool MagnetometerSensorChannelInterface::dataReceivedImpl()
{
    QVector<CalibratedMagneticFieldData> values;
    if(!read<CalibratedMagneticFieldData>(values))
        return false;
    if(!frameAvailableConnected || values.size() == 1)
    {
        foreach(const CalibratedMagneticFieldData& data, values)
            emit dataAvailable(MagneticField(data));
    }
    else
    {
        QVector<MagneticField> realValues;
        realValues.reserve(values.size());
        foreach(const CalibratedMagneticFieldData& data, values)
            realValues.push_back(MagneticField(data));
        emit frameAvailable(realValues);
    }
    return true;
}

void MagnetometerSensorChannelInterface::connectNotify(const char* signal)
{
    if(QLatin1String(signal) == SIGNAL(frameAvailable(QVector<MagneticField>)))
        frameAvailableConnected = true;
    dbusConnectNotify(signal);
}

QDBusReply<void> MagnetometerSensorChannelInterface::reset()
{
    return call(QDBus::Block, QLatin1String("reset"));
}

MagneticField MagnetometerSensorChannelInterface::magneticField()
{
    return getAccessor<MagneticField>("magneticField");
}
