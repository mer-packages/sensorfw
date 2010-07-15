/**
   @file abstractsensor_i.cpp
   @brief Base class for sensor interface

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
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

#include "sensormanagerinterface.h"
#include "abstractsensor_i.h"

AbstractSensorChannelInterface::AbstractSensorChannelInterface(const QString &path, const char* interfaceName, int sessionId) :
        QDBusAbstractInterface(SERVICE_NAME, path, interfaceName, QDBusConnection::systemBus(), 0),
        sessionId_(sessionId), running_(false), interval_(0), standbyOverride_(false), socketReader_(NULL)
{
#ifdef USE_SOCKET
    socketReader_ = new SocketReader(this);

    if (!socketReader_->initiateConnection(sessionId_)) {
        setError(SClientSocketError, "Socket connection failed.");
        // TODO: Invalidate!
    }
#endif

    qDBusRegisterMetaType<DataRange>();

    Q_ASSERT ( isValid() );
}

AbstractSensorChannelInterface::~AbstractSensorChannelInterface()
{
    if ( isValid() )
    {
        release();
    }
    
#ifdef USE_SOCKET
    if (socketReader_) {

        if (!socketReader_->dropConnection()) {
            setError(SClientSocketError, "Socket disconnect failed.");
        }
        delete socketReader_;
    }
#endif
}

bool AbstractSensorChannelInterface::release()
{
    // ToDo: note that after release this interace becomes invalid (this should be handled correctly)
    //qDebug()  << __PRETTY_FUNCTION__ << "Releasing sensor" << id() << "session" << sessionId_;
    bool result = SensorManagerInterface::instance().releaseInterface(id(), sessionId_);
    return result;
}

void AbstractSensorChannelInterface::setError(SensorError errorCode, const QString& errorString)
{
    errorCode_   = errorCode;
    errorString_ = errorString;

    //emit errorSignal(errorCode);
}

QDBusReply<void> AbstractSensorChannelInterface::start()
{
    return start(sessionId_);
}

QDBusReply<void> AbstractSensorChannelInterface::stop()
{
    return stop(sessionId_);
}

QDBusReply<void> AbstractSensorChannelInterface::start(int sessionId)
{
    clearError();
    
    if (running_) {
        return QDBusReply<void>();
    }
    running_ = true;

#ifdef USE_SOCKET
    if (socketReader_) {
        connect(socketReader_->socket(), SIGNAL(readyRead()), this, SLOT(dataReceived()));
    }
#endif

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId);
    QDBusReply<void> returnValue = callWithArgumentList(QDBus::Block, QLatin1String("start"), argumentList);

    /// Send interval request when started.
    setInterval(sessionId, interval_);

    return returnValue;
}

QDBusReply<void> AbstractSensorChannelInterface::stop(int sessionId)
{
    clearError();

    if (!running_) {
        return QDBusReply<void>();
    }
    running_ = false ;

#ifdef USE_SOCKET
    if (socketReader_) {
        disconnect(socketReader_->socket(), SIGNAL(readyRead()), this, SLOT(dataReceived()));
    }
#endif

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

    for (int i = 0; i < retVal.value(); i++) {
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
    call(QDBus::Block, QLatin1String("requestDataRange"), qVariantFromValue(sessionId_), qVariantFromValue(range));
}

void AbstractSensorChannelInterface::removeDataRangeRequest()
{
    clearError();
    call(QDBus::Block, QLatin1String("removeDataRangeRequest"), qVariantFromValue(sessionId_));
}

QList<DataRange> AbstractSensorChannelInterface::getAvailableIntervals()
{
    clearError();

    QList<DataRange> intervals;

    QDBusReply<int> retVal = call(QDBus::Block, QLatin1String("getIntervalCount"));

    for (int i = 0; i < retVal.value(); i++) {
        QDBusReply<DataRange> interval = call(QDBus::Block, QLatin1String("getAvailableInterval"), qVariantFromValue(i));
        intervals.append(interval.value());
    }
    return intervals;
}
