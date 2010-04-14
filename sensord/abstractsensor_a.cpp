/**
   @file abstractsensor_a.cpp
   @brief D-Bus adaptor base class for sensors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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

#include "abstractsensor_a.h"
#include "sfwerror.h"

bool AbstractSensorChannelAdaptor::isValid() const
{
    return qvariant_cast< bool >(parent()->property("isValid"));
}

int AbstractSensorChannelAdaptor::errorCodeInt() const
{
    // note: enums can not be transferred over D-BUS
    return static_cast<SensorError>(qvariant_cast< int >(parent()->property("errorCodeInt")));
}

QString AbstractSensorChannelAdaptor::errorString() const
{
    return qvariant_cast< QString >(parent()->property("errorString"));
}

QString AbstractSensorChannelAdaptor::description() const
{
    // get the value of property description
    return qvariant_cast< QString >(parent()->property("description"));
}

QString AbstractSensorChannelAdaptor::id() const
{
    // get the value of property id
    return qvariant_cast< QString >(parent()->property("id"));
}

//~ int AbstractSensorChannelAdaptor::interval() const
//~ {
    //~ // get the value of property interval
    //~ return qvariant_cast< int >(parent()->property("interval"));
//~ }
//~ 
//~ void AbstractSensorChannelAdaptor::setInterval(int value)
//~ {
    //~ // set the value of property interval
    //~ parent()->setProperty("interval", value);
//~ }

/*
SensorState AbstractSensorChannelAdaptor::state() const
{
    // get the value of property state
    return qvariant_cast< SensorState >(parent()->property("state"));
}
*/

QString AbstractSensorChannelAdaptor::type() const
{
    // get the value of property type
    return qvariant_cast< QString >(parent()->property("type"));
}

void AbstractSensorChannelAdaptor::start(int sessionId)
{
    // handle method call local.Sensor.start
    QMetaObject::invokeMethod(parent(), "start", Q_ARG(int, sessionId));
}

void AbstractSensorChannelAdaptor::stop(int sessionId)
{
    // handle method call local.Sensor.stop
    QMetaObject::invokeMethod(parent(), "stop", Q_ARG(int, sessionId));
}

void AbstractSensorChannelAdaptor::setInterval(int sessionId, int value)
{
    QMetaObject::invokeMethod(parent(), "setInterval", Q_ARG(int, sessionId), Q_ARG(int, value));
}

void AbstractSensorChannelAdaptor::setStandbyOverride(int sessionId, bool value)
{
    QMetaObject::invokeMethod(parent(), "setStandbyOverride", Q_ARG(int, sessionId), Q_ARG(bool, value));
}
