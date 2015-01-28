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

/**
 * @brief D-Bus adaptor base class for sensors
 *
 * Sensor DBus facade for handling remote method invocations. It instance
 * has associated AbstractSensorChannel to which this object delegates
 * calls.
 */
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

public:
    /**
     * Destructor.
     */
    virtual ~AbstractSensorChannelAdaptor() {}

protected:
    /**
     * Constructor.
     *
     * @param parent parent object.
     */
    AbstractSensorChannelAdaptor(QObject *parent);

private:
    /**
     * Previously occured error code.
     *
     * @return error code.
     */
    int errorCodeInt() const;

    /**
     * Pointer to parent channel.
     *
     * @return point to parent channel.
     */
    AbstractSensorChannel* node() const;

public Q_SLOTS: // METHODS

    /** AbstractSensorChannel::isValid() */
    bool isValid() const;

    /** AbstractSensorChannel::errorString() */
    QString errorString() const;

    /** AbstractSensorChannel::description() */
    QString description() const;

    /** AbstractSensorChannel::id() */
    QString id() const;

    /** AbstractSensorChannel::interval() */
    unsigned int interval() const;

    /** AbstractSensorChannel::standbyOverride() */
    bool standbyOverride() const;

    /** AbstractSensorChannel::bufferInterval() */
    unsigned int bufferInterval() const;

    /** AbstractSensorChannel::bufferSize() */
    unsigned int bufferSize() const;

    /** AbstractSensorChannel::type() */
    QString type() const;

    /** AbstractSensorChannel::start(int) */
    void start(int sessionId);

    /** AbstractSensorChannel::stop(int) */
    void stop(int sessionId);

    /** AbstractSensorChannel::setInterval(int, int)
     *
     *  Will also configure interval for the data connection.
     */
    void setInterval(int sessionId, int value);

    /** AbstractSensorChannel::getAvailableIntervals() */
    DataRangeList getAvailableIntervals();

    /** AbstractSensorChannel::setDefaultInteval(int) */
    bool setDefaultInterval(int sessionId);

    /** AbstractSensorChannel::setStandbyOverride(int, bool) */
    bool setStandbyOverride(int sessionId, bool value);

    /** AbstractSensorChannel::setDownsampling(int, bool) */
    void setDownsampling(int sessionId, bool value);

    /** AbstractSensorChannel::isValid(int, unsigned int)
     *
     *  Will also configure buffer interval for the data connection.
     */
    void setBufferInterval(int sessionId, unsigned int value);

    /** AbstractSensorChannel::isValid(int, unsigned int)
     *
     *  Will also configure buffer size for the data connection.
     */
    void setBufferSize(int sessionId, unsigned int value);

    /** AbstractSensorChannel::getAvailableBufferIntervals() */
    IntegerRangeList getAvailableBufferIntervals() const;

    /** AbstractSensorChannel::getAvailableBufferSizes() */
    IntegerRangeList getAvailableBufferSizes() const;

    /** AbstractSensorChannel::getAvailableDataRanges() */
    DataRangeList getAvailableDataRanges();

    /** AbstractSensorChannel::getCurrentDataRange() */
    DataRange getCurrentDataRange();

    /** AbstractSensorChannel::requestDataRange(int, DataRange) */
    void requestDataRange(int sessionId, DataRange range);

    /** AbstractSensorChannel::removeDataRangeRequest(int) */
    void removeDataRangeRequest(int sessionId);

    /** AbstractSensorChannel::setDataRangeIndex(int, int) */
    bool setDataRangeIndex(int sessionId, int rangeIndex);

    /** AbstractSensorChannel::hwBuffering() */
    bool hwBuffering() const;

Q_SIGNALS:
    /** AbstractSensorChannel::propertyChanged(name) */
    void propertyChanged(const QString& name);
};

#endif // ABSTRACTSENSORADAPTOR_H
