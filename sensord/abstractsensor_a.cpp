/**
   @file abstractsensor_a.cpp
   @brief D-Bus adaptor base class for sensors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
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

#include "abstractsensor_a.h"
#include "sfwerror.h"
#include <sensormanager.h>
#include <sockethandler.h>

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

unsigned int AbstractSensorChannelAdaptor::interval() const
{
    return qvariant_cast< unsigned int >(parent()->property("interval"));
}

unsigned int AbstractSensorChannelAdaptor::bufferInterval() const
{
    return qvariant_cast<unsigned int>(parent()->property("bufferInterval"));
}

unsigned int AbstractSensorChannelAdaptor::bufferSize() const
{
    return qvariant_cast<unsigned int>(parent()->property("bufferSize"));
}

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
    bool success;
    QMetaObject::invokeMethod(parent(), "setIntervalRequest", Q_RETURN_ARG(bool, success), Q_ARG(const int, sessionId), Q_ARG(const unsigned int, (unsigned int)value));
    SensorManager::instance().socketHandler().setInterval(sessionId, value);
}

bool AbstractSensorChannelAdaptor::standbyOverride() const
{
    return qvariant_cast< bool >(parent()->property("standbyOverride"));
}

bool AbstractSensorChannelAdaptor::setStandbyOverride(int sessionId, bool value)
{
    bool success;
    QMetaObject::invokeMethod(parent(), "setStandbyOverrideRequest", Q_RETURN_ARG(bool, success), Q_ARG(int, sessionId), Q_ARG(bool, value));
    return success;
}

DataRangeList AbstractSensorChannelAdaptor::getAvailableDataRanges()
{
    NodeBase* node = dynamic_cast<NodeBase*>(parent());
    if(node)
    {
        return node->getAvailableDataRanges();
    }
    return DataRangeList();
}

DataRange AbstractSensorChannelAdaptor::getCurrentDataRange()
{
    DataRangeRequest rangeRequest;
    QMetaObject::invokeMethod(parent(), "getCurrentDataRange", Q_RETURN_ARG(DataRangeRequest, rangeRequest));
    return rangeRequest.range_;
}

void AbstractSensorChannelAdaptor::requestDataRange(int sessionId, DataRange range)
{
    QMetaObject::invokeMethod(parent(), "requestDataRange", Q_ARG(int, sessionId), Q_ARG(DataRange, range));
}

void AbstractSensorChannelAdaptor::removeDataRangeRequest(int sessionId)
{
    QMetaObject::invokeMethod(parent(), "removeDataRangeRequest", Q_ARG(int, sessionId));
}

DataRangeList AbstractSensorChannelAdaptor::getAvailableIntervals()
{
    NodeBase* node = dynamic_cast<NodeBase*>(parent());
    if(node)
    {
        return node->getAvailableIntervals();
    }
    return DataRangeList();
}

bool AbstractSensorChannelAdaptor::setDefaultInterval(int sessionId)
{
    bool success;
    QMetaObject::invokeMethod(parent(), "requestDefaultInterval", Q_RETURN_ARG(bool, success), Q_ARG(int, sessionId));
    SensorManager::instance().socketHandler().clearInterval(sessionId);
    return success;
}

void AbstractSensorChannelAdaptor::setBufferInterval(int sessionId, unsigned int value)
{
    // TODO: propagate value for the actual nodes to get driver buffering enabled
    SensorManager::instance().socketHandler().setBufferInterval(sessionId, value);
}

void AbstractSensorChannelAdaptor::setBufferSize(int sessionId, unsigned int value)
{
    // TODO: propagate value for the actual nodes to get driver buffering enabled
    if(isInRange(value, getAvailableBufferIntervals()))
        SensorManager::instance().socketHandler().setBufferSize(sessionId, value);
}

IntegerRangeList AbstractSensorChannelAdaptor::getAvailableBufferIntervals() const
{
    IntegerRangeList list;
    list.push_back(qMakePair<unsigned int, unsigned int>(0, 60000));
    return list;
}

IntegerRangeList AbstractSensorChannelAdaptor::getAvailableBufferSizes() const
{
    NodeBase* node = dynamic_cast<NodeBase*>(parent());
    if(node)
    {
        return node->getAvailableBufferSizes();
    }
    return IntegerRangeList();
}
