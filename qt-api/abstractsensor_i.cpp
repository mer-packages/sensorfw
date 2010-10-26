/**
   @file abstractsensor_i.cpp
   @brief Base class for sensor interface

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#include "sensormanagerinterface.h"
#include "abstractsensor_i.h"

struct AbstractSensorChannelInterface::AbstractSensorChannelInterfaceImpl
{
    AbstractSensorChannelInterfaceImpl(QObject* parent, int sessionId);

    SensorError errorCode_;
    QString errorString_;
    int sessionId_;
    int interval_;
    SocketReader socketReader_;
    bool running_;
    bool standbyOverride_;
};

AbstractSensorChannelInterface::AbstractSensorChannelInterfaceImpl::AbstractSensorChannelInterfaceImpl(QObject* parent, int sessionId) :
    sessionId_(sessionId),
    interval_(0),
    socketReader_(parent),
    running_(false),
    standbyOverride_(false)
{
}

AbstractSensorChannelInterface::AbstractSensorChannelInterface(const QString& path, const char* interfaceName, int sessionId) :
    QDBusAbstractInterface(SERVICE_NAME, path, interfaceName, QDBusConnection::systemBus(), 0),
    pimpl_(new AbstractSensorChannelInterfaceImpl(this, sessionId))
{
    if (!pimpl_->socketReader_.initiateConnection(sessionId)) {
        setError(SClientSocketError, "Socket connection failed.");
        // TODO: Invalidate!
    }

    qDBusRegisterMetaType<DataRange>();

    Q_ASSERT ( isValid() );
}

AbstractSensorChannelInterface::~AbstractSensorChannelInterface()
{
    if ( isValid() )
        release();
    if (!pimpl_->socketReader_.dropConnection())
        setError(SClientSocketError, "Socket disconnect failed.");
    delete pimpl_;
}

bool AbstractSensorChannelInterface::release()
{
    // ToDo: note that after release this interace becomes invalid (this should be handled correctly)
    return SensorManagerInterface::instance().releaseInterface(id(), pimpl_->sessionId_);
}

void AbstractSensorChannelInterface::setError(SensorError errorCode, const QString& errorString)
{
    pimpl_->errorCode_   = errorCode;
    pimpl_->errorString_ = errorString;

    //emit errorSignal(errorCode);
}

QDBusReply<void> AbstractSensorChannelInterface::start()
{
    return start(pimpl_->sessionId_);
}

QDBusReply<void> AbstractSensorChannelInterface::stop()
{
    return stop(pimpl_->sessionId_);
}

QDBusReply<void> AbstractSensorChannelInterface::start(int sessionId)
{
    clearError();

    if (pimpl_->running_) {
        return QDBusReply<void>();
    }
    pimpl_->running_ = true;

    connect(pimpl_->socketReader_.socket(), SIGNAL(readyRead()), this, SLOT(dataReceived()));

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId);
    QDBusReply<void> returnValue = callWithArgumentList(QDBus::Block, QLatin1String("start"), argumentList);

    if (pimpl_->standbyOverride_)
    {
        setStandbyOverride(sessionId, true);
    }
    /// Send interval request when started.
    setInterval(sessionId, pimpl_->interval_);

    return returnValue;
}

QDBusReply<void> AbstractSensorChannelInterface::stop(int sessionId)
{
    clearError();

    if (!pimpl_->running_) {
        return QDBusReply<void>();
    }
    pimpl_->running_ = false ;

    disconnect(pimpl_->socketReader_.socket(), SIGNAL(readyRead()), this, SLOT(dataReceived()));
    setStandbyOverride(sessionId, false);
    /// Drop interval requests when stopped
    setInterval(sessionId, 0);

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId);
    return callWithArgumentList(QDBus::Block, QLatin1String("stop"), argumentList);
}

QDBusReply<void> AbstractSensorChannelInterface::setInterval(int sessionId, int value)
{
    clearError();

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId) << qVariantFromValue(value);
    return callWithArgumentList(QDBus::Block, QLatin1String("setInterval"), argumentList);
}

QDBusReply<bool> AbstractSensorChannelInterface::setStandbyOverride(int sessionId, bool value)
{
    clearError();

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId) << qVariantFromValue(value);
    return callWithArgumentList(QDBus::Block, QLatin1String("setStandbyOverride"), argumentList);
}

QList<DataRange> AbstractSensorChannelInterface::getAvailableDataRanges()
{
    clearError();

    QList<DataRange> dataRanges;

    QDBusReply<int> retVal = call(QDBus::Block, QLatin1String("getDataRangeCount"));

    for (int i = 0; i < retVal.value(); ++i) {
        QDBusReply<DataRange> range = call(QDBus::Block, QLatin1String("getAvailableDataRange"), qVariantFromValue(i));
        dataRanges.append(range.value());
    }
    return dataRanges;
}

DataRange AbstractSensorChannelInterface::getCurrentDataRange()
{
    clearError();
    QDBusReply<DataRange> retVal = call(QDBus::Block, QLatin1String("getCurrentDataRange"));
    return retVal.value();
}

void AbstractSensorChannelInterface::requestDataRange(DataRange range)
{
    clearError();
    call(QDBus::Block, QLatin1String("requestDataRange"), qVariantFromValue(pimpl_->sessionId_), qVariantFromValue(range));
}

void AbstractSensorChannelInterface::removeDataRangeRequest()
{
    clearError();
    call(QDBus::Block, QLatin1String("removeDataRangeRequest"), qVariantFromValue(pimpl_->sessionId_));
}

QList<DataRange> AbstractSensorChannelInterface::getAvailableIntervals()
{
    clearError();

    QList<DataRange> intervals;

    QDBusReply<int> retVal = call(QDBus::Block, QLatin1String("getIntervalCount"));

    for (int i = 0; i < retVal.value(); ++i) {
        QDBusReply<DataRange> interval = call(QDBus::Block, QLatin1String("getAvailableInterval"), qVariantFromValue(i));
        intervals.append(interval.value());
    }
    return intervals;
}

int AbstractSensorChannelInterface::sessionId() const
{
    return pimpl_->sessionId_;
}

SensorError AbstractSensorChannelInterface::errorCode() const
{
    // TODO: This solution may introduce problems, if errors are
    //       not cleared before another happens.
    if (pimpl_->errorCode_ != SNoError) {
        return pimpl_->errorCode_;
    }
    return static_cast<SensorError>(errorCodeInt());
}

QString AbstractSensorChannelInterface::errorString() const
{
    if (pimpl_->errorCode_ != SNoError) {
        return pimpl_->errorString_;
    }
    return qvariant_cast<QString>(internalPropGet("errorString"));
}

QString AbstractSensorChannelInterface::description() const
{
    return qvariant_cast<QString>(internalPropGet("description"));
}

QString AbstractSensorChannelInterface::id() const
{
    return qvariant_cast<QString>(internalPropGet("id"));
}

int AbstractSensorChannelInterface::interval() const
{
    return qvariant_cast<int>(internalPropGet("interval"));
}

void AbstractSensorChannelInterface::setInterval(int value)
{
    pimpl_->interval_ = value;
    if (pimpl_->running_)
        setInterval(pimpl_->sessionId_, value);
}

bool AbstractSensorChannelInterface::standbyOverride() const
{
    return pimpl_->standbyOverride_ || qvariant_cast<bool>(internalPropGet("standbyOverride"));
}

bool AbstractSensorChannelInterface::setStandbyOverride(bool override)
{
    pimpl_->standbyOverride_ = override;
    return setStandbyOverride(pimpl_->sessionId_, override);
}

QString AbstractSensorChannelInterface::type() const
{
    return qvariant_cast<QString>(internalPropGet("type"));
}

int AbstractSensorChannelInterface::errorCodeInt() const
{
    return static_cast<SensorManagerError>(qvariant_cast<int>(internalPropGet("errorCodeInt")));
}

void AbstractSensorChannelInterface::clearError()
{
    pimpl_->errorCode_ = SNoError;
    pimpl_->errorString_.clear();
}

void AbstractSensorChannelInterface::dataReceived()
{
}

bool AbstractSensorChannelInterface::read(void* buffer, int size)
{
    return pimpl_->socketReader_.read(buffer, size);
}
