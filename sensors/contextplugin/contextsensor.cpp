/**
   @file contextsensor.cpp
   @brief ContextSensor
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>

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

#include "contextsensor.h"
#include "serviceinfo.h"
#include "sensord/sensormanager.h"
#include "sensord/logging.h"

ContextSensorChannel::ContextSensorChannel(const QString& id) :
    AbstractSensorChannel(id), service(QDBusConnection::systemBus()),
    orientationBin(service), compassBin(NULL)
{
    // Attempt to load compasschain
    if (SensorManager::instance().loadPlugin("compasschain"))
    {
        compassBin = new CompassBin(service);
    } else {
        sensordLogD() << "Loading of 'compasschain' failed, no Location.Heading available";

        // Creating as dummy to provide the service with value 'unknown'
        // rather than miss the service.
        compassBin = new CompassBin(service, false);
    }

    isValid_ = true;
}

ContextSensorChannel::~ContextSensorChannel()
{
    if (compassBin)
    {
        delete compassBin;
    }
}
