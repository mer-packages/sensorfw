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
#include <QVector>
#include <QString>

#include "sfwerror.h"
#include "serviceinfo.h"
#include "socketreader.h"
#include "datatypes/datarange.h"

/**
 * Base-class for client facades of different sensor types.
 */
class AbstractSensorChannelInterface : public QObject
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
    Q_PROPERTY(unsigned int bufferInterval READ bufferInterval WRITE setBufferInterval)
    Q_PROPERTY(unsigned int bufferSize READ bufferSize WRITE setBufferSize)
    Q_PROPERTY(bool hwBuffering READ hwBuffering)
    Q_PROPERTY(bool downsampling READ downsampling WRITE setDownsampling)

public:

    /**
     * Destructor
     */
    virtual ~AbstractSensorChannelInterface();

    /**
     * Release the sensor instance.
     *
     * @return true always.
     * @deprecated Session lifecycle ends by destructing this object.
     */
    bool release();

    /**
     * Get ID of the current session.
     *
     * @return current session ID.
     */
    int sessionId() const;

    /**
     * Get error code of occured local or remote error.
     *
     * @return error code.
     */
    SensorError errorCode();

    /**
     * Get error description of occured local or remote error.
     *
     * @return error description.
     */
    QString errorString();

    /**
     * Get description of the sensor.
     *
     * @return description of the sensor.
     */
    QString description();

    /**
     * Get ID of the sensor.
     *
     * @return ID of the sensor.
     */
    QString id();

    /**
     * Get used sensor sampling interval (in millisecs).
     * If sensor has been started (by #start()) effective interval is
     * returned.
     *
     * @return used sampling interval (in millisecs)
     */
    int interval();

    /**
     * Set sensor sampling interval (in millisecs).
     * Value "0" will clear previously set interval.
     * Supported intervals are listed by #getAvailableIntervals().
     *
     * @param value sampling interval (in millisecs).
     */
    void setInterval(int value);

    /**
     * Is standby-override enabled or not.
     * Standby-override is used to keep sensor enabled when display
     * is blanked.
     *
     * @return enabled or not.
     */
    bool standbyOverride();

    /**
     * Enable or disable standby-override.
     * Standby-override is used to keep sensor enabled when display
     * is blanked.
     *
     * @param override standby-override state
     * @return true if succesfully set.
     */
    bool setStandbyOverride(bool override);

    /**
     * Used buffer interval. Buffer interval defines the timeout for
     * buffered data to be flushed unless the buffer is filled before it.
     *
     * @return interval in millisecs.
     */
    unsigned int bufferInterval();

    /**
     * Set buffer interval. Buffer interval defines the timeout for
     * buffered data to be flushed unless the buffer is filled before it.
     * Supported intervals are listed by #getAvailableBufferIntervals().
     *
     * @param value interval in millisecs.
     */
    void setBufferInterval(unsigned int value);

    /**
     * Is downsampling enabled or not.
     * Downsampling is used to fit requested measurement interval
     * #setInterval(int) to effective interval.
     *
     * @return downsampling state.
     */
    bool downsampling();

    /**
     * Enable or disable downsampling.
     * Downsampling is used to fit requested measurement interval
     * #setInterval(int) to effective interval. Not all sensors support
     * downsampling so the call will fail for those.
     *
     * @param value enable or disable downsampling.
     * @return was downsampling state succesfully changed.
     */
    bool setDownsampling(bool value);

    /**
     * Returns list of available buffer interval ranges.
     *
     * @return The list of supported buffer interval ranges.
     */
    IntegerRangeList getAvailableBufferIntervals();

    /**
     * Get used buffer size. Buffer size is used to control how many
     * samples are collected before signaling application about them.
     *
     * @return used buffer size.
     */
    unsigned int bufferSize();

    /**
     * Set buffer size. Buffer size is used to control how many
     * samples are collected before signaling application about them.
     *
     * @param value buffer size.
     */
    void setBufferSize(unsigned int value);

    /**
     * Returns list of available buffer sizes. The list is ordered by
     * efficiency of the size.
     *
     * @return The list of supported buffer sizes ordered by efficiency.
     */
    IntegerRangeList getAvailableBufferSizes();

    /**
     * Textual description about sensor type.
     *
     * @return sensor type description.
     */
    QString type();

    /**
     * Start sensor. This will cause necessary resources to be
     * acquired so the sensor readings can be received.
     *
     * @return object from which the success of call can be seen.
     */
    virtual QDBusReply<void> start();

    /**
     * Stop sensor. This will cause acquired resourced to be released.
     *
     * @return object from which the success of call can be seen.
     */
    virtual QDBusReply<void> stop();

    /**
     * Get the list of available intervals ranges for the sensor.
     *
     * @return List of available intervals ranges.
     */
    DataRangeList getAvailableIntervals();

    /**
     * Get the list of available data ranges for the sensor.
     *
     * @return List of available data ranges.
     */
    DataRangeList getAvailableDataRanges();

    /**
     * Get current used data range.
     *
     * @return currently used data range.
     */
    DataRange getCurrentDataRange();

    /**
     * Request data range. Usable ranges are listed by
     * #getAvailableDataRanges().
     *
     * @param range requested range.
     */
    void requestDataRange(DataRange range);

    /**
     * Remove set data range request.
     */
    void removeDataRangeRequest();

    /**
     * Request data range by using index which points to the list
     * returned by #getAvailableDataRanges().
     *
     * @param dataRangeIndex requeted data range index.
     * @return false if given index is invalid.
     */
    bool setDataRangeIndex(int dataRangeIndex);

    /**
     * Does the sensor driver support buffering or not.
     *
     * @return Does the sensor driver support buffering or not.
     */
    bool hwBuffering();

    /**
     * Does the current instance have valid connection established
     * to sensor daemon.
     *
     * @return is the established connection valid.
     */
    bool isValid() const;

private:
    /**
     * Set error information.
     *
     * @param errorCode error code.
     * @param errorString error description.
     */
    void setError(SensorError errorCode, const QString& errorString);

    /**
     * Clear set error information.
     */
    void clearError();

    /**
     * Get socket reader instance.
     */
    SocketReader& getSocketReader() const;

private Q_SLOTS: // METHODS

    void displayStateChanged(bool displayState);

    /**
     * Set interval to session.
     *
     * @param sessionId session ID.
     * @param value interval.
     * @return DBus reply.
     */
    QDBusReply<void> setInterval(int sessionId, int value);

    /**
     * Set standby-override to session.
     *
     * @param sessionId session ID.
     * @param value standby-override.
     * @return DBus reply.
     */
    QDBusReply<bool> setStandbyOverride(int sessionId, bool value);

    /**
     * Set buffer interval to session.
     *
     * @param sessionId session ID.
     * @param value buffer interval.
     * @return DBus reply.
     */
    QDBusReply<void> setBufferInterval(int sessionId, unsigned int value);

    /**
     * Set buffer size to session.
     *
     * @param sessionId session ID.
     * @param value buffer size.
     * @return DBus reply.
     */
    QDBusReply<void> setBufferSize(int sessionId, unsigned int value);

    /**
     * Set downsampling to session.
     *
     * @param sessionId session ID.
     * @param value downsampling.
     * @return DBus reply.
     */
    QDBusReply<void> setDownsampling(int sessionId, bool value);

    /**
     * Start sensor for session.
     *
     * @param sessionId session ID.
     * @return DBus reply.
     */
    QDBusReply<void> start(int sessionId);

    /**
     * Stop sensor for session.
     *
     * @param sessionId session ID.
     * @return DBus reply.
     */
    QDBusReply<void> stop(int sessionId);

    /**
     * Callback for pending data in data socket.
     */
    void dataReceived();

protected:
    /**
     * Constructor.
     *
     * @param path DBus object path.
     * @param interfaceName DBus object name.
     * @param sessionId Session ID.
     */
    AbstractSensorChannelInterface(const QString& path, const char* interfaceName, int sessionId);

    /**
     * Read data from socket into buffer.
     *
     * @param buffer Pointer to buffer where to write.
     * @param size Number of bytes to read.
     */
    bool read(void* buffer, int size);

    /**
     * Read data from socket into passed container.
     *
     * @tparam Type to which to convert raw data.
     * @param values Container for data.
     * @return was read successful.
     */
    template<typename T>
    bool read(QVector<T>& values);

    /**
     * Callback for subclasses in which they must read their expected data
     * from socket.
     *
     * @return was read successful.
     */
    virtual bool dataReceivedImpl() = 0;

    /**
     * Utility for calling DBus methods from current connection which
     * return value and take no args.
     *
     * @tparam return type.
     * @param name method name.
     * @return called method return value.
     */
    template<typename T>
    T getAccessor(const char* name);

    /**
     * Utility for calling DBus methods from current connection which
     * return nothing and take one arg.
     *
     * @tparam argument type.
     * @param name method name.
     * @param value method argument.
     */
    template<typename T>
    void setAccessor(const char* name, const T& value);

    /**
     * Wrapper for function of QDBusAbstractInterface.
     * For more details see QDBusAbstractInterface docs.
     */
    QDBusMessage call(QDBus::CallMode mode,
                      const QString& method,
                      const QVariant& arg1 = QVariant(),
                      const QVariant& arg2 = QVariant(),
                      const QVariant& arg3 = QVariant(),
                      const QVariant& arg4 = QVariant(),
                      const QVariant& arg5 = QVariant(),
                      const QVariant& arg6 = QVariant(),
                      const QVariant& arg7 = QVariant(),
                      const QVariant& arg8 = QVariant());

    /**
     * Wrapper for function of QDBusAbstractInterface.
     * For more details see QDBusAbstractInterface docs.
     */
    QDBusMessage callWithArgumentList(QDBus::CallMode mode, const QString& method, const QList<QVariant>& args);

    /**
     * Wrapper for function of QDBusAbstractInterface.
     * For more details see QDBusAbstractInterface docs.
     */
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    void dbusConnectNotify(const char* signal);
#else
    void dbusConnectNotify(const QMetaMethod& signal);
#endif
protected slots:
    void startFinished(QDBusPendingCallWatcher *watch);
    void stopFinished(QDBusPendingCallWatcher *watch);

    void setIntervalFinished(QDBusPendingCallWatcher *watch);
    void setBufferIntervalFinished(QDBusPendingCallWatcher *watch);
    void setBufferSizeFinished(QDBusPendingCallWatcher *watch);
    void setStandbyOverrideFinished(QDBusPendingCallWatcher *watch);
    void setDownsamplingFinished(QDBusPendingCallWatcher *watch);
    void setDataRangeIndexFinished(QDBusPendingCallWatcher *watch);


private:
    struct AbstractSensorChannelInterfaceImpl;

    AbstractSensorChannelInterfaceImpl* pimpl_; /**< private impl pointer */
};

template<typename T>
bool AbstractSensorChannelInterface::read(QVector<T>& values)
{
    return getSocketReader().read(values);
}

template<typename T>
T AbstractSensorChannelInterface::getAccessor(const char* name)
{
    QDBusReply<T> reply(call(QDBus::Block, QLatin1String(name)));
    if(!reply.isValid())
    {
        qDebug() << "Failed to get '" << name << "' from sensord: " << reply.error().message();
        return T();
    }
    return reply.value();
}

template<typename T>
void AbstractSensorChannelInterface::setAccessor(const char* name, const T& value)
{
    QDBusReply<void> reply(call(QDBus::NoBlock, QLatin1String(name), qVariantFromValue(value)));
    if(!reply.isValid())
    {
        qDebug() << "Failed to set '" << name << " = " << value << "' to sensord: " << reply.error().message();
    }
}

namespace local {
    typedef ::AbstractSensorChannelInterface AbstractSensor;
}

#endif
