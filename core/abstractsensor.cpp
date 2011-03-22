/**
   @file abstractsensor.cpp
   @brief Base class for sensors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>

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
    sensordLogC() << "SensorError: " <<  errorString;

    errorCode_   = errorCode;
    errorString_ = errorString;

    emit errorSignal(errorCode);
}

bool AbstractSensorChannel::start(int sessionId)
{
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


bool AbstractSensorChannel::writeToSession(int sessionId, const void* source, int size)
{
    if (!(SensorManager::instance().write(sessionId, source, size))) {
        sensordLogD() << "AbstractSensor failed to write to session " << sessionId;
        return false;
    }

    return true;
}

bool AbstractSensorChannel::writeToClients(const void* source, int size)
{
    bool ret = true;
    foreach(const int& sessionId, activeSessions_) {
        ret &= writeToSession(sessionId, source, size);
    }
    return ret;
}

bool AbstractSensorChannel::downsampleAndPropagate(const TimedXyzData& data, TimedXyzDownsampleBuffer& buffer)
{
    bool ret = true;
    unsigned int currentInterval = getInterval();
    foreach(const int& sessionId, activeSessions_)
    {
        if(!downsamplingEnabled(sessionId))
        {
            ret &= writeToSession(sessionId, (const void *)& data, sizeof(data));
            continue;
        }
        unsigned int sessionInterval = getInterval(sessionId);
        int bufferSize = (sessionInterval < currentInterval ) ? 1 : sessionInterval / currentInterval;

        QList<TimedXyzData>& samples(buffer[sessionId]);
        samples.push_back(data);

        for(QList<TimedXyzData>::iterator it = samples.begin(); it != samples.end(); ++it)
        {

            if(samples.size() > bufferSize ||
               data.timestamp_ - it->timestamp_ > 2000000)
            {
                it = samples.erase(it);
                if(it == samples.end())
                    break;
            } else
            {
                break;
            }
        }

        if(samples.size() < bufferSize)
        {
            continue;
        }

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

        if (writeToSession(sessionId, (const void*)& downsampled, sizeof(downsampled)))
        {
            samples.clear();
        } else
        {
            ret = false;
        }
    }

    return ret;
}

bool AbstractSensorChannel::downsampleAndPropagate(const CalibratedMagneticFieldData& data, MagneticFieldDownsampleBuffer& buffer)
{
    bool ret = true;
    unsigned int currentInterval = getInterval();
    foreach(const int& sessionId, activeSessions_)
    {
        if(!downsamplingEnabled(sessionId))
        {
            ret &= writeToSession(sessionId, (const void *)& data, sizeof(data));
            continue;
        }
        unsigned int sessionInterval = getInterval(sessionId);
        int bufferSize = (sessionInterval < currentInterval ) ? 1 : sessionInterval / currentInterval;

        QList<CalibratedMagneticFieldData>& samples(buffer[sessionId]);
        samples.push_back(data);

        for(QList<CalibratedMagneticFieldData>::iterator it = samples.begin(); it != samples.end(); ++it)
        {
            if(samples.size() > bufferSize ||
               data.timestamp_ - it->timestamp_ > 2000000)
            {
                it = samples.erase(it);
                if(it == samples.end())
                    break;
            } else
            {
                break;
            }
        }

        if(samples.size() < bufferSize)
            continue;
        long x = 0;
        long y = 0;
        long z = 0;
        long rx = 0;
        long ry = 0;
        long rz = 0;
        foreach(const CalibratedMagneticFieldData& data, samples)
        {
            x += data.x_;
            y += data.y_;
            z += data.z_;
            rx += data.rx_;
            ry += data.ry_;
            rz += data.rz_;
        }
        CalibratedMagneticFieldData downsampled(data.timestamp_,
                                                x / samples.count(),
                                                y / samples.count(),
                                                z / samples.count(),
                                                rx / samples.count(),
                                                ry / samples.count(),
                                                rz / samples.count(),
                                                data.level_);
        sensordLogT() << "Downsampled for session " << sessionId << ": " << downsampled.x_ << ", " << downsampled.y_ << ", " << downsampled.z_ << downsampled.rx_ << ", " << downsampled.ry_ << ", " << downsampled.rz_;

        if (writeToSession(sessionId, (const void*)& downsampled, sizeof(downsampled)))
        {
            samples.clear();
        } else
        {
            ret = false;
        }

    }
    return ret;
}


void AbstractSensorChannel::setDownsamplingEnabled(int sessionId, bool value)
{
    if(downsamplingSupported())
    {
        sensordLogT() << "Downsampling state for session " << sessionId << ": " << value;
        downsampling_[sessionId] = value;
    }
}

bool AbstractSensorChannel::downsamplingEnabled(int sessionId)
{
    QMap<int, bool>::const_iterator it(downsampling_.find(sessionId));
    if(it == downsampling_.end())
        return downsamplingSupported();
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
