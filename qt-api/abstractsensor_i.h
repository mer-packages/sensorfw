/**
   @file abstractsensor_i.h
   @brief Base class for sensor interface

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Semi Malinen <semi.malinen@nokia.com
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

#ifndef ABSTRACTSENSOR_I_H
#define ABSTRACTSENSOR_I_H

#include <QtDBus/QtDBus>
#include <QList>
#include <QString>

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
    Q_DISABLE_COPY(AbstractSensorChannelInterface)
    Q_PROPERTY(int sessionId READ sessionId)
    Q_PROPERTY(SensorError errorCode READ errorCode)
    Q_PROPERTY(QString errorString READ errorString)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(int interval READ interval WRITE setInterval)
    Q_PROPERTY(bool standbyOverride READ standbyOverride WRITE setStandbyOverride)
    Q_PROPERTY(QString type READ type)
    Q_PROPERTY(int errorCodeInt READ errorCodeInt)
    Q_PROPERTY(int bufferInterval READ bufferInterval WRITE setBufferInterval)
    Q_PROPERTY(int bufferSize READ bufferSize WRITE setBufferSize)

public:
    virtual ~AbstractSensorChannelInterface();

    bool release();

    int sessionId() const;

    SensorError errorCode() const;

    QString errorString() const;

    QString description() const;

    QString id() const;

    int interval() const;
    void setInterval(int value);

    bool standbyOverride() const;
    bool setStandbyOverride(bool override);

    int bufferInterval() const;
    void setBufferInterval(int value);

    int bufferSize() const;
    void setBufferSize(int value);

    QString type() const;

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

private:
    int errorCodeInt() const;
    void setError(SensorError errorCode, const QString& errorString);
    void clearError();

protected Q_SLOTS:
    QDBusReply<void> setInterval(int sessionId, int value);
    QDBusReply<bool> setStandbyOverride(int sessionId, bool value);

private Q_SLOTS: // METHODS
    QDBusReply<void> start(int sessionId);
    QDBusReply<void> stop(int sessionId);

    virtual void dataReceived();

Q_SIGNALS: // SIGNALS
    void propertyChanged(const QString& name);

protected:
    AbstractSensorChannelInterface(const QString& path, const char* interfaceName, int sessionId);

    bool read(void* buffer, int size);

private:
    struct AbstractSensorChannelInterfaceImpl;

    AbstractSensorChannelInterfaceImpl* pimpl_;
};

namespace local {
  typedef ::AbstractSensorChannelInterface AbstractSensor;
}

#endif
