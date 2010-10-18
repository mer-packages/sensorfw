/**
   @file abstractsensor.cpp
   @brief Base class for sensors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
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

#include <QStringList>
#include <QVariant>

#include "sensormanager.h"
#include "idutils.h"
#include "logging.h"

#include "abstractsensor.h"

AbstractSensorChannel::AbstractSensorChannel(const QString& id)
    : //, state_(STOPPED),
    errorCode_(SNoError)
    , isValid_(false)
    , cnt_(0)
{
    id_ = getCleanId(id);
}

void AbstractSensorChannel::setError(SensorError errorCode, const QString& errorString)
{
    sensordLogC() << "SensorError:" <<  errorString;

    errorCode_   = errorCode;
    errorString_ = errorString;

    emit errorSignal(errorCode);
}

bool AbstractSensorChannel::start(int sessionId) {
    if (!(activeSessions_.contains(sessionId))) {
        activeSessions_.append(sessionId);
    }

    requestDefaultInterval(sessionId);

    return start();
}

bool AbstractSensorChannel::start()
{
    //Q_ASSERT( interval_ > 0 );

    //setState( STARTED );
    // TODO

    // TODO: Use active list as run refcount().

    // return true only on change from 0->1
    if (++cnt_ == 1) return true;
    return false;
}

bool AbstractSensorChannel::stop(int sessionId) {
    activeSessions_.removeAll(sessionId);

    if(stop())
    {
        removeIntervalRequest(sessionId);
        return true;
    }
    return false;
}

bool AbstractSensorChannel::stop()
{
    //setState( STOPPED );
    // TODO

    // TODO: Use active list as run refcount().

    // Return true only on change from 1->0
    if (--cnt_ == 0) return true;
    if (cnt_ < 0) cnt_ = 0;
    return false;
}


bool AbstractSensorChannel::writeToClients(const void* source, int size)
{
    foreach(int sessionId, activeSessions_) {
        if (!(SensorManager::instance().write(sessionId, source, size))) {
            sensordLogD() << "AbstractSensor failed to write to session " << sessionId;
            return false;
        }
    }
    return true;
}
