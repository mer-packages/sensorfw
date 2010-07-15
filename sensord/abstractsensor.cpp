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

#include "sensord/sensormanager.h"
#include "idutils.h"

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

int AbstractSensorChannel::interval() const
{
    QString kbName = "kbslideradaptor";
    if ((adaptorList_.size() == 1) || (adaptorList_.size() == 2 && adaptorList_.contains(kbName)))
    {
        int index = 0;
        if (adaptorList_.at(index) == kbName) {
            index++;
        }

        return SensorManager::instance().propertyHandler().getHighestValue("interval", adaptorList_.at(index));
    }

    sensordLogC() << "Running a sensor (" << name_ <<") that does not reimplement interval() function but uses more than one adaptor (or none).";

    if (adaptorList_.size() == 0) {
        return 0;
    }

    return SensorManager::instance().propertyHandler().getHighestValue("interval", adaptorList_.at(0));
}



bool AbstractSensorChannel::start(int sessionId) {
#ifdef USE_SOCKET
    if (!(activeSessions_.contains(sessionId))) {
        activeSessions_.append(sessionId);
    }
#endif
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
#ifdef USE_SOCKET
    activeSessions_.removeAll(sessionId);
#endif

    return stop();
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

void AbstractSensorChannel::setInterval(int sessionId, int value)
{
    // Verify that requested value is in list of allowed values.
    bool validRequest = false;
    for (int i = 0; i < intervalList_.size(); i++) {
        if (intervalList_.at(i).min <= value &&
            intervalList_.at(i).max >= value) {
                validRequest = true;
            }
    }

    if (!validRequest) {
        sensordLogD() << "Requested invalid interval" << value;
    }

    // Make an interval request for all listed adaptors.
    foreach (QString adaptor, adaptorList_) {
        SensorManager::instance().propertyHandler().setRequest("interval", adaptor, sessionId, value);
    }

    // TODO: Signal only when rate has actually changed
    signalPropertyChanged("interval");
}

bool AbstractSensorChannel::setStandbyOverride(int sessionId, bool value)
{
    foreach (QString adaptor, adaptorList_) {
        SensorManager::instance().propertyHandler().setRequest("standbyOverride", adaptor, sessionId, value);
    }
    return true;
}

QList<DataRange> AbstractSensorChannel::getAvailableIntervals()
{
    return intervalList_;
}

#ifdef USE_SOCKET
bool AbstractSensorChannel::writeToClients(const void* source, int size)
{
    foreach(int sessionId, activeSessions_) {
        if (!(SensorManager::instance().write(sessionId, source, size))) {
            qDebug() << "[AbstractSensor]: Failed to write to session" << sessionId << activeSessions_;
            return false;
        }
    }
    return true;
}
#endif
