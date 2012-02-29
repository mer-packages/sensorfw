/**
   @file sensormanagerinterface.cpp
   @brief Interface for SensorManager

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
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

#include "serviceinfo.h"
#include "idutils.h"
#include "sensormanagerinterface.h"

SensorManagerInterface* SensorManagerInterface::ifc_ = 0;
QMutex SensorManagerInterface::mutex_;

SensorManagerInterface::SensorManagerInterface()
  : LocalSensorManagerInterface( SERVICE_NAME, OBJECT_PATH, QDBusConnection::systemBus() )
{
}

SensorManagerInterface& SensorManagerInterface::instance()
{
    if ( !ifc_ )
    {
        ifc_ = new SensorManagerInterface();
        if ( !ifc_->isValid() )
        {
            qDebug() << "Failed to get sensor manager interface: " << ifc_->lastError().message();
        }
    }

    return *ifc_;
}

bool SensorManagerInterface::registeredAndCorrectClassName(const QString& id, const QString& className ) const
{
    QString cleanId = getCleanId(id);
    return ( sensorInterfaceMap_.contains(cleanId) ) && ( sensorInterfaceMap_[cleanId].type == className );
}

AbstractSensorChannelInterface* SensorManagerInterface::interface(const QString& id)
{
    if ( !sensorInterfaceMap_.contains(id) )
    {
        qDebug() << "Requested sensor id '" << id << "' interface not known";
        return 0;
    }

    SensorManagerInterface& sm = SensorManagerInterface::instance();
    AbstractSensorChannelInterface* ifc = 0;
    int sessionId = sm.requestSensor(id);
    if ( sessionId >= 0 ) // sensor is available
    {
        QString cleanId = getCleanId(id);
        ifc = sensorInterfaceMap_[cleanId].sensorInterfaceFactory(cleanId, sessionId);
    }
    else
    {
        qDebug() << "Requested sensor id '" << id << "' interface not granted";
    }

    return ifc;
}

bool SensorManagerInterface::releaseInterface(const QString& id, int sessionId)
{
    QString cleanId = getCleanId(id);

    QDBusReply<bool> reply = LocalSensorManagerInterface::releaseSensor(cleanId, sessionId);
    if ( !reply.isValid() )
    {
        qDebug() << "Failed to release sensor '" << id << "' interface for session '" << sessionId << "'. Error: " << reply.error().message();
        return false;
    }
    return reply.value();
}
