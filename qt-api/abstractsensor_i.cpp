/**
   @file abstractsensor_i.cpp
   @brief Base class for sensor interface

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>

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

struct AbstractSensorChannelInterface::AbstractSensorChannelInterfaceImpl : public QDBusAbstractInterface
{
    AbstractSensorChannelInterfaceImpl(QObject* parent, int sessionId, const QString& path, const char* interfaceName);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    void dbusConnectNotify(const char* signal) { QDBusAbstractInterface::connectNotify(signal); }
#else
    void dbusConnectNotify(const QMetaMethod& signal) { QDBusAbstractInterface::connectNotify(signal); }
#endif
    SensorError errorCode_;
    QString errorString_;
    int sessionId_;
    int interval_;
    unsigned int bufferInterval_;
    unsigned int bufferSize_;
    SocketReader socketReader_;
    bool running_;
    bool standbyOverride_;
    bool downsampling_;
};

AbstractSensorChannelInterface::AbstractSensorChannelInterfaceImpl::AbstractSensorChannelInterfaceImpl(QObject* parent, int sessionId, const QString& path, const char* interfaceName) :
    QDBusAbstractInterface(SERVICE_NAME, path, interfaceName, QDBusConnection::systemBus(), 0),
    errorCode_(SNoError),
    errorString_(""),
    sessionId_(sessionId),
    interval_(0),
    bufferInterval_(0),
    bufferSize_(1),
    socketReader_(parent),
    running_(false),
    standbyOverride_(false),
    downsampling_(true)
{
}

AbstractSensorChannelInterface::AbstractSensorChannelInterface(const QString& path, const char* interfaceName, int sessionId) :
    pimpl_(new AbstractSensorChannelInterfaceImpl(this, sessionId, path, interfaceName))
{
    if (!pimpl_->socketReader_.initiateConnection(sessionId)) {
        setError(SClientSocketError, "Socket connection failed.");
    }
}

AbstractSensorChannelInterface::~AbstractSensorChannelInterface()
{
    if ( pimpl_->isValid() )
        SensorManagerInterface::instance().releaseInterface(id(), pimpl_->sessionId_);
    if (!pimpl_->socketReader_.dropConnection())
        setError(SClientSocketError, "Socket disconnect failed.");
    delete pimpl_;
}

SocketReader& AbstractSensorChannelInterface::getSocketReader() const
{
    return pimpl_->socketReader_;
}

bool AbstractSensorChannelInterface::release()
{
    return true;
}

void AbstractSensorChannelInterface::setError(SensorError errorCode, const QString& errorString)
{
    pimpl_->errorCode_   = errorCode;
    pimpl_->errorString_ = errorString;
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
    QDBusReply<void> returnValue = pimpl_->callWithArgumentList(QDBus::Block, QLatin1String("start"), argumentList);

    setStandbyOverride(sessionId, pimpl_->standbyOverride_);
    setInterval(sessionId, pimpl_->interval_);
    setBufferInterval(sessionId, pimpl_->bufferInterval_);
    setBufferSize(sessionId, pimpl_->bufferSize_);
    setDownsampling(pimpl_->sessionId_, pimpl_->downsampling_);

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

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId);
    return pimpl_->callWithArgumentList(QDBus::Block, QLatin1String("stop"), argumentList);
}

QDBusReply<void> AbstractSensorChannelInterface::setInterval(int sessionId, int value)
{
    clearError();

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId) << qVariantFromValue(value);
    return pimpl_->callWithArgumentList(QDBus::Block, QLatin1String("setInterval"), argumentList);
}

QDBusReply<void> AbstractSensorChannelInterface::setBufferInterval(int sessionId, unsigned int value)
{
    clearError();

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId) << qVariantFromValue(value);
    return pimpl_->callWithArgumentList(QDBus::Block, QLatin1String("setBufferInterval"), argumentList);
}

QDBusReply<void> AbstractSensorChannelInterface::setBufferSize(int sessionId, unsigned int value)
{
    clearError();

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId) << qVariantFromValue(value);
    return pimpl_->callWithArgumentList(QDBus::Block, QLatin1String("setBufferSize"), argumentList);
}

QDBusReply<bool> AbstractSensorChannelInterface::setStandbyOverride(int sessionId, bool value)
{
    clearError();

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId) << qVariantFromValue(value);
    return pimpl_->callWithArgumentList(QDBus::Block, QLatin1String("setStandbyOverride"), argumentList);
}

DataRangeList AbstractSensorChannelInterface::getAvailableDataRanges()
{
    return getAccessor<DataRangeList>("getAvailableDataRanges");
}

DataRange AbstractSensorChannelInterface::getCurrentDataRange()
{
    return getAccessor<DataRange>("getCurrentDataRange");
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

DataRangeList AbstractSensorChannelInterface::getAvailableIntervals()
{
    return getAccessor<DataRangeList>("getAvailableIntervals");
}

IntegerRangeList AbstractSensorChannelInterface::getAvailableBufferIntervals()
{
    return getAccessor<IntegerRangeList>("getAvailableBufferIntervals");
}

IntegerRangeList AbstractSensorChannelInterface::getAvailableBufferSizes()
{
    return getAccessor<IntegerRangeList>("getAvailableBufferSizes");
}

bool AbstractSensorChannelInterface::hwBuffering()
{
    return getAccessor<bool>("hwBuffering");
}

int AbstractSensorChannelInterface::sessionId() const
{
    return pimpl_->sessionId_;
}

SensorError AbstractSensorChannelInterface::errorCode()
{
    if (pimpl_->errorCode_ != SNoError) {
        return pimpl_->errorCode_;
    }
    return static_cast<SensorError>(getAccessor<int>("errorCodeInt"));
}

QString AbstractSensorChannelInterface::errorString()
{
    if (pimpl_->errorCode_ != SNoError)
        return pimpl_->errorString_;
    return getAccessor<QString>("errorString");
}

QString AbstractSensorChannelInterface::description()
{
    return getAccessor<QString>("description");
}

QString AbstractSensorChannelInterface::id()
{
    return getAccessor<QString>("id");
}

int AbstractSensorChannelInterface::interval()
{
    if (pimpl_->running_)
        return static_cast<int>(getAccessor<unsigned int>("interval"));
    return pimpl_->interval_;
}

void AbstractSensorChannelInterface::setInterval(int value)
{
    pimpl_->interval_ = value;
    if (pimpl_->running_)
        setInterval(pimpl_->sessionId_, value);
}

unsigned int AbstractSensorChannelInterface::bufferInterval()
{
    if (pimpl_->running_)
        return getAccessor<unsigned int>("bufferInterval");
    return pimpl_->bufferInterval_;
}

void AbstractSensorChannelInterface::setBufferInterval(unsigned int value)
{
    pimpl_->bufferInterval_ = value;
    if (pimpl_->running_)
        setBufferInterval(pimpl_->sessionId_, value);
}

unsigned int AbstractSensorChannelInterface::bufferSize()
{
    if (pimpl_->running_)
        return getAccessor<unsigned int>("bufferSize");
    return pimpl_->bufferSize_;
}

void AbstractSensorChannelInterface::setBufferSize(unsigned int value)
{
    pimpl_->bufferSize_ = value;
    if (pimpl_->running_)
        setBufferSize(pimpl_->sessionId_, value);
}

bool AbstractSensorChannelInterface::standbyOverride()
{
    if (pimpl_->running_)
        return getAccessor<unsigned int>("standbyOverride");
    return pimpl_->standbyOverride_;
}

bool AbstractSensorChannelInterface::setStandbyOverride(bool override)
{
    pimpl_->standbyOverride_ = override;
    if (pimpl_->running_)
        return setStandbyOverride(pimpl_->sessionId_, override);
    return true;
}

QString AbstractSensorChannelInterface::type()
{
    return getAccessor<QString>("type");
}

void AbstractSensorChannelInterface::clearError()
{
    pimpl_->errorCode_ = SNoError;
    pimpl_->errorString_.clear();
}

void AbstractSensorChannelInterface::dataReceived()
{
    do
    {
        if(!dataReceivedImpl())
            return;
    } while(pimpl_->socketReader_.socket()->bytesAvailable());
}

bool AbstractSensorChannelInterface::read(void* buffer, int size)
{
    return pimpl_->socketReader_.read(buffer, size);
}

bool AbstractSensorChannelInterface::setDataRangeIndex(int dataRangeIndex)
{
    clearError();
    QDBusReply<bool> dbusReply = pimpl_->call(QLatin1String("setDataRangeIndex"),
                                              qVariantFromValue(pimpl_->sessionId_), qVariantFromValue(dataRangeIndex));
    if (dbusReply.isValid())
        return dbusReply.value();
    return false;
}

QDBusMessage AbstractSensorChannelInterface::call(QDBus::CallMode mode,
                                                  const QString& method,
                                                  const QVariant& arg1,
                                                  const QVariant& arg2,
                                                  const QVariant& arg3,
                                                  const QVariant& arg4,
                                                  const QVariant& arg5,
                                                  const QVariant& arg6,
                                                  const QVariant& arg7,
                                                  const QVariant& arg8)
{
    return pimpl_->call(mode, method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

QDBusMessage AbstractSensorChannelInterface::callWithArgumentList(QDBus::CallMode mode, const QString& method, const QList<QVariant>& args)
{
    return pimpl_->call(mode, method, args);
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
void AbstractSensorChannelInterface::dbusConnectNotify(const char* signal)
#else
void AbstractSensorChannelInterface::dbusConnectNotify(const QMetaMethod& signal)
#endif
{
    pimpl_->dbusConnectNotify(signal);
}

bool AbstractSensorChannelInterface::isValid() const
{
    return pimpl_->isValid();
}

bool AbstractSensorChannelInterface::downsampling()
{
    return pimpl_->downsampling_;
}

bool AbstractSensorChannelInterface::setDownsampling(bool value)
{
    pimpl_->downsampling_ = value;
    return setDownsampling(pimpl_->sessionId_, value).isValid();
}

QDBusReply<void> AbstractSensorChannelInterface::setDownsampling(int sessionId, bool value)
{
    clearError();

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sessionId) << qVariantFromValue(value);
    return pimpl_->callWithArgumentList(QDBus::Block, QLatin1String("setDownsampling"), argumentList);
}
