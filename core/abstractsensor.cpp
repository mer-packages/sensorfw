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
#include "sockethandler.h"
#include "idutils.h"
#include "logging.h"

#include "abstractsensor.h"

AbstractSensorChannel::AbstractSensorChannel(const QString& id) :
    errorCode_(SNoError),
    isValid_(false),
    cnt_(0)
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
    if (++cnt_ == 1) return true;
    return false;
}

bool AbstractSensorChannel::stop(int sessionId)
{
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
    if (--cnt_ == 0) return true;
    if (cnt_ < 0) cnt_ = 0;
    return false;
}

bool AbstractSensorChannel::writeToClients(const void* source, int size)
{
    foreach(const int& sessionId, activeSessions_) {
        if (!(SensorManager::instance().write(sessionId, source, size))) {
            sensordLogD() << "AbstractSensor failed to write to session " << sessionId;
            return false;
        }
    }
    return true;
}

bool AbstractSensorChannel::downsampleAndPropagate(const TimedXyzData& data, TimedXyzDownsampleBuffer& buffer)
{
    foreach(const int& sessionId, activeSessions_)
    {
        if(!downsamplingEnabled(sessionId))
        {
            if (!(SensorManager::instance().write(sessionId, (const void*)(&data), sizeof(data))))
            {
                sensordLogD() << "AbstractSensor failed to write to session " << sessionId;
                return false;
            }
            return true;
        }
        unsigned int currentInterval = interval();
        unsigned int sessionInterval = getInterval(sessionId);
        int bufferSize = currentInterval / sessionInterval;
        if(!bufferSize)
            bufferSize = 1;
        QList<TimedXyzData>& samples(buffer[sessionId]);
        samples.push_back(data);
        if(samples.size() > 1)
        {
            for(QList<TimedXyzData>::iterator it = samples.begin(); it != samples.end(); ++it)
            {
                if(samples.size() > bufferSize)
                {
                    it = samples.erase(it);
                    if(it == samples.end())
                        break;
                }
            }
        }
        if(samples.size() < bufferSize)
            return false;
        long x = 0;
        long y = 0;
        long z = 0;
        foreach(const TimedXyzData& data, samples)
        {
            x += data.x_;
            y += data.y_;
            z += data.z_;
        }
        TimedXyzData downsampled(data.timestamp_,
                                 x / samples.count(),
                                 y / samples.count(),
                                 z / samples.count());
        sensordLogT() << "Downsampled for session " << sessionId << ": " << downsampled.x_ << ", " << downsampled.y_ << ", " << downsampled.z_;
        if (!(SensorManager::instance().write(sessionId, (const void*)(&downsampled), sizeof(downsampled))))
        {
            sensordLogD() << "AbstractSensor failed to write to session " << sessionId;
            return false;
        }
        samples.clear();
    }
    return true;
}

void AbstractSensorChannel::setDownsamplingEnabled(int sessionId, bool value)
{
    downsampling_[sessionId] = value;
}

bool AbstractSensorChannel::downsamplingEnabled(int sessionId)
{
    QMap<int, bool>::const_iterator it(downsampling_.find(sessionId));
    if(it == downsampling_.end())
        return false;
    return it.value() && getInterval(sessionId);
}

bool AbstractSensorChannel::downsamplingSupported() const
{
    return false;
}

void AbstractSensorChannel::removeSession(int sessionId)
{
    downsampling_.take(sessionId);
    NodeBase::removeSession(sessionId);
}
