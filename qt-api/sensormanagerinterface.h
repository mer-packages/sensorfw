/**
   @file sensormanagerinterface.h
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

#ifndef SENSORMANAGERINTERFACE_H
#define SENSORMANAGERINTERFACE_H

#include "sensormanager_i.h"
#include "abstractsensor_i.h"

typedef AbstractSensorChannelInterface* (*SensorInterfaceFactoryMethod)(const QString& id, int sessionId);

struct SensorInterfaceEntry {
    SensorInterfaceFactoryMethod sensorInterfaceFactory;
    QString                      type; // could be an integer to safe memory
};

class SensorManagerInterface : public LocalSensorManagerInterface
{
    Q_OBJECT

public:
    static SensorManagerInterface& instance();

    template<class SensorInterfaceType>
    void registerSensorInterface(const QString& sensorName);

    AbstractSensorChannelInterface* interface(const QString& id);
    bool releaseInterface(const QString& id, int sessionId);

    bool registeredAndCorrectClassName(const QString& id, const QString& className ) const;

protected:
    SensorManagerInterface();
    virtual ~SensorManagerInterface() {}

    QMap<QString, SensorInterfaceEntry> sensorInterfaceMap_;

    static SensorManagerInterface* ifc_;
};

template<class SensorInterfaceType>
void SensorManagerInterface::registerSensorInterface(const QString& sensorName)
{
    sensorInterfaceMap_[sensorName].sensorInterfaceFactory = SensorInterfaceType::factoryMethod;
    sensorInterfaceMap_[sensorName].type = SensorInterfaceType::staticMetaObject.className();
}

#endif // SENSORMANAGERINTERFACE_H
