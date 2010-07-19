/**
   @file abstractsensor_i.h
   @brief Base class for sensor interface

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Semi Malinen <semi.malinen@nokia.com
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

#ifndef ABSTRACTSENSOR_I_H
#define ABSTRACTSENSOR_I_H

#include <QtDBus/QtDBus>
#include <QLocalSocket>
#include <QList>

#include "sfwerror.h"
#include "serviceinfo.h"
#include "socketreader.h"
#include "datatypes/datarange.h"

/*
 * Proxy class for interface local.Sensor
 */
class AbstractSensorChannelInterface: public QDBusAbstractInterface
{
    Q_OBJECT

public:
    virtual ~AbstractSensorChannelInterface();

    bool release();

    Q_PROPERTY(int sessionId READ sessionId)
    int sessionId() const { return sessionId_; }

    Q_PROPERTY(SensorError errorCode READ errorCode)
    SensorError errorCode() const
    {
        // TODO: This solution may introduce problems, if errors are
        //       not cleared before another happens.
        if (errorCode_ != SNoError) {
            return errorCode_;
        }
        return static_cast<SensorError>(errorCodeInt());
    }

    Q_PROPERTY(QString errorString READ errorString)
    QString errorString() const
    {
        if (errorCode_ != SNoError) {
            return errorString_;
        }
        return qvariant_cast< QString >(internalPropGet("errorString"));
    }

    Q_PROPERTY(QString description READ description)
    inline QString description() const
    { return qvariant_cast< QString >(internalPropGet("description")); }

    Q_PROPERTY(QString id READ id)
    inline QString id() const
    { return qvariant_cast< QString >(internalPropGet("id")); }

    Q_PROPERTY(int interval READ interval WRITE setInterval)
    inline int interval() const
    { return qvariant_cast< int >(internalPropGet("interval")); }
    inline void setInterval(int value)
    {
        interval_ = value;
        /// Only set when running
        if (running_) {
            setInterval(sessionId_, value);
        }
    }

    Q_PROPERTY(bool standbyOverride READ standbyOverride WRITE setStandbyOverride);
    inline bool standbyOverride() const
    {
        return qvariant_cast< bool >(internalPropGet("standbyOverride"));;
    }
    inline bool setStandbyOverride(bool override)
    {
        return setStandbyOverride(sessionId_, override);
    }

    /*
    Q_PROPERTY(SensorState state READ state)
    inline SensorState state() const
    { return qvariant_cast< SensorState >(internalPropGet("state")); }
    */

    Q_PROPERTY(QString type READ type)
    inline QString type() const
    { return qvariant_cast< QString >(internalPropGet("type")); }

    virtual QDBusReply<void> start();
    virtual QDBusReply<void> stop();

    /**
     * Get the list of available intervals for the sensor.
     *
     * @return List of available intervals (or interval ranges)
     */
    QList<DataRange> getAvailableIntervals();

    QList<DataRange> getAvailableDataRanges();
    DataRange getCurrentDataRange();
    void requestDataRange(DataRange range);
    void removeDataRangeRequest();

private: // this exists as a hack because enums cannot be marshalled over D-BUS
    Q_PROPERTY(int errorCodeInt READ errorCodeInt)
    int errorCodeInt() const
    { return static_cast<SensorManagerError>(qvariant_cast< int >(internalPropGet("errorCodeInt"))); }

    void setError(SensorError errorCode, const QString& errorString);
    void clearError() { errorCode_ = SNoError; errorString_.clear(); }


public Q_SLOTS:

protected Q_SLOTS:
    QDBusReply<void> setInterval(int sessionId, int value);
    QDBusReply<bool> setStandbyOverride(int sessionId, bool value);

private Q_SLOTS: // METHODS
    QDBusReply<void> start(int sessionId);
    QDBusReply<void> stop(int sessionId);

    virtual void dataReceived() {}

Q_SIGNALS: // SIGNALS
    // TODO add state back
    /*
    void stateChanged(SensorState state);
    */
    void propertyChanged(const QString& name);

protected :
    AbstractSensorChannelInterface(const QString &path, const char* interfaceName, int sessionId);

    int sessionId_;
    bool running_;
    int interval_;

    SocketReader* socketReader_;

private:
    SensorError errorCode_;
    QString errorString_;
};

namespace local {
  typedef ::AbstractSensorChannelInterface AbstractSensor;
}

#endif
