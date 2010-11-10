/**
   @file magnetometersensor_i.cpp
   @brief Interface for MagnetometerSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

QDBusAbstractInterface* MagnetometerSensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    // ToDo: see which arguments can be made explicit
    return new MagnetometerSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}

MagnetometerSensorChannelInterface::MagnetometerSensorChannelInterface(const QString& path, int sessionId) :
    AbstractSensorChannelInterface(path, MagnetometerSensorChannelInterface::staticInterfaceName, sessionId),
    frameAvailableConnected(false)
{
}

const MagnetometerSensorChannelInterface* MagnetometerSensorChannelInterface::listenInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    // ToDo: can conversion from class name to type string be automated?
    if ( !sm.registeredAndCorrectClassName( id, MagnetometerSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<const MagnetometerSensorChannelInterface*>(sm.listenInterface(id));
}

MagnetometerSensorChannelInterface* MagnetometerSensorChannelInterface::controlInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, MagnetometerSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<MagnetometerSensorChannelInterface*>(sm.controlInterface(id));
}

void MagnetometerSensorChannelInterface::dataReceived()
{
    QVector<CalibratedMagneticFieldData> values;
    while (read<CalibratedMagneticFieldData>(values))
    {
        if(values.size() == 1)
            emit dataAvailable(MagneticField(values.back()));
        else
        {
            if(frameAvailableConnected)
            {
                QVector<MagneticField> realValues;
                realValues.resize(values.size());
                foreach(CalibratedMagneticFieldData data, values)
                    realValues.push_back(MagneticField(data));
                emit frameAvailable(realValues);
                values.clear();
            }
            else
            {
                foreach(CalibratedMagneticFieldData data, values)
                    emit dataAvailable(MagneticField(data));
            }
        }
    }
}

void MagnetometerSensorChannelInterface::connectNotify(const char* signal)
{
    if(QLatin1String(signal) == SIGNAL(frameAvailable(const QVector<MagneticField>)))
        frameAvailableConnected = true;
}

QDBusReply<void> MagnetometerSensorChannelInterface::reset()
{
    QList<QVariant> argumentList;
    return callWithArgumentList(QDBus::Block, QLatin1String("reset"), argumentList);
}

MagneticField MagnetometerSensorChannelInterface::magneticField() const
{
    return qvariant_cast< MagneticField >(internalPropGet("magneticField"));
}
