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

AbstractSensorChannelAdaptor::AbstractSensorChannelAdaptor(QObject *parent) :
    QDBusAbstractAdaptor(parent)
{
    setAutoRelaySignals(false); //disabling signals since no public client API supports the use of these
}

bool AbstractSensorChannelAdaptor::isValid() const
{
    return node()->isValid();
}

int AbstractSensorChannelAdaptor::errorCodeInt() const
{
    return node()->errorCodeInt();
}

QString AbstractSensorChannelAdaptor::errorString() const
{
    return node()->errorString();
}

QString AbstractSensorChannelAdaptor::description() const
{
    return node()->description();
}

QString AbstractSensorChannelAdaptor::id() const
{
    return node()->id();
}

unsigned int AbstractSensorChannelAdaptor::interval() const
{
    return node()->getInterval();
}

unsigned int AbstractSensorChannelAdaptor::bufferInterval() const
{
    return node()->bufferInterval();
}

unsigned int AbstractSensorChannelAdaptor::bufferSize() const
{
    return node()->bufferSize();
}

bool AbstractSensorChannelAdaptor::hwBuffering() const
{
    bool hwBuffering = false;
    node()->getAvailableBufferSizes(hwBuffering);
    return hwBuffering;
}

QString AbstractSensorChannelAdaptor::type() const
{
    return node()->type();
}

void AbstractSensorChannelAdaptor::start(int sessionId)
{
    node()->start(sessionId);
}

void AbstractSensorChannelAdaptor::stop(int sessionId)
{
    node()->stop(sessionId);
}

void AbstractSensorChannelAdaptor::setInterval(int sessionId, int value)
{
    node()->setIntervalRequest(sessionId, value);
    SensorManager::instance().socketHandler().setInterval(sessionId, value);
}

bool AbstractSensorChannelAdaptor::standbyOverride() const
{
    return node()->standbyOverride();
}

bool AbstractSensorChannelAdaptor::setStandbyOverride(int sessionId, bool value)
{
    return node()->setStandbyOverrideRequest(sessionId, value);
}

DataRangeList AbstractSensorChannelAdaptor::getAvailableDataRanges()
{
    return node()->getAvailableDataRanges();
}

DataRange AbstractSensorChannelAdaptor::getCurrentDataRange()
{
    return node()->getCurrentDataRange().range;
}

void AbstractSensorChannelAdaptor::requestDataRange(int sessionId, DataRange range)
{
    node()->requestDataRange(sessionId, range);
}

void AbstractSensorChannelAdaptor::removeDataRangeRequest(int sessionId)
{
    node()->removeDataRangeRequest(sessionId);
}

DataRangeList AbstractSensorChannelAdaptor::getAvailableIntervals()
{
    return node()->getAvailableIntervals();
}

bool AbstractSensorChannelAdaptor::setDefaultInterval(int sessionId)
{
    bool ok = node()->requestDefaultInterval(sessionId);
    SensorManager::instance().socketHandler().clearInterval(sessionId);
    return ok;
}

void AbstractSensorChannelAdaptor::setBufferInterval(int sessionId, unsigned int value)
{
    bool hwBuffering = false;
    node()->getAvailableBufferIntervals(hwBuffering);
    if(hwBuffering)
    {
        if(value == 0)
            node()->clearBufferInterval(sessionId);
        else
            node()->setBufferInterval(sessionId, value);
        value = 0;
    }
    if(value == 0)
        SensorManager::instance().socketHandler().clearBufferInterval(sessionId);
    else
        SensorManager::instance().socketHandler().setBufferInterval(sessionId, value);
}

void AbstractSensorChannelAdaptor::setBufferSize(int sessionId, unsigned int value)
{
    bool hwBuffering = false;
    node()->getAvailableBufferSizes(hwBuffering);
    if(hwBuffering)
    {
        if(value == 0)
            node()->clearBufferSize(sessionId);
        else
            node()->setBufferSize(sessionId, value);
    }
    if(value == 0)
        SensorManager::instance().socketHandler().clearBufferSize(sessionId);
    else
        SensorManager::instance().socketHandler().setBufferSize(sessionId, value);
}

IntegerRangeList AbstractSensorChannelAdaptor::getAvailableBufferIntervals() const
{
    bool dummy;
    return node()->getAvailableBufferIntervals(dummy);
}

IntegerRangeList AbstractSensorChannelAdaptor::getAvailableBufferSizes() const
{
    bool dummy;
    return node()->getAvailableBufferSizes(dummy);
}

AbstractSensorChannel* AbstractSensorChannelAdaptor::node() const
{
    return dynamic_cast<AbstractSensorChannel*>(parent());
}

bool AbstractSensorChannelAdaptor::setDataRangeIndex(int sessionId, int rangeIndex)
{
    return node()->setDataRangeIndex(sessionId, rangeIndex);
}

void AbstractSensorChannelAdaptor::setDownsampling(int sessionId, bool value)
{
    SensorManager::instance().socketHandler().setDownsampling(sessionId, value);
}
