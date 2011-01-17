/**
   @file abstractsensor_a.h
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

#ifndef ABSTRACTSENSORADAPTOR_H
#define ABSTRACTSENSORADAPTOR_H

#include <QtDBus/QtDBus>
#include "abstractsensor.h"
#include "datatypes/datarange.h"

class AbstractSensorChannelAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractSensorChannelAdaptor)
    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(QString errorString READ errorString)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(unsigned int interval READ interval)
    Q_PROPERTY(bool standbyOverride READ standbyOverride)
    Q_PROPERTY(unsigned int bufferInterval READ bufferInterval)
    Q_PROPERTY(unsigned int bufferSize READ bufferSize)
    Q_PROPERTY(QString type READ type)
    Q_PROPERTY(int errorCodeInt READ errorCodeInt)
    Q_PROPERTY(bool hwBuffering READ hwBuffering)
    // Q_PROPERTY(SensorState state READ state) //TODO: put state back

public:
    virtual ~AbstractSensorChannelAdaptor() {}

protected:
    AbstractSensorChannelAdaptor(QObject *parent);

public:
    bool isValid() const;
    QString errorString() const;
    QString description() const;
    QString id() const;
    unsigned int interval() const;
    bool standbyOverride() const;
    unsigned int bufferInterval() const;
    unsigned int bufferSize() const;
    QString type() const;
    // SensorState state() const; //TODO: put state back

private: // PROPERTIES
    int errorCodeInt() const;
    AbstractSensorChannel* node() const;

public Q_SLOTS: // METHODS
    void start(int sessionId);
    void stop(int sessionId);

    void setInterval(int sessionId, int value);
    DataRangeList getAvailableIntervals();
    bool setDefaultInterval(int sessionId);

    bool setStandbyOverride(int sessionId, bool value);

    void setBufferInterval(int sessionId, unsigned int value);
    void setBufferSize(int sessionId, unsigned int value);
    IntegerRangeList getAvailableBufferIntervals() const;
    IntegerRangeList getAvailableBufferSizes() const;

    DataRangeList getAvailableDataRanges();
    DataRange getCurrentDataRange();
    void requestDataRange(int sessionId, DataRange range);
    void removeDataRangeRequest(int sessionId);

Q_SIGNALS: // SIGNALS
    void propertyChanged(const QString& name);

public:
    bool hwBuffering() const;
};

#endif // ABSTRACTSENSORADAPTOR_H
