/**
   @file sockethandler.h
   @brief SocketHandler

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Shenghua <ext-shenghua.1.liu@nokia.com>

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

#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QList>
#include <QMutex>
#include <QLocalSocket>
#include <sys/time.h>

class QLocalServer;

/**
 * Class contains data for single sensor session related data socket
 * connection.
 */
class SessionData : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SessionData)

public:
    /**
     * Constructor.
     *
     * @param socket Established socket connection. SessionData will take
     *               the ownership of it.
     * @param parent Parent object.
     */
    SessionData(QLocalSocket* socket, QObject* parent = 0);

    /**
     * Destructor.
     */
    virtual ~SessionData();

    /**
     * Write data to socket. The data might be queued before written.
     *
     * @param source Source from where to write.
     * @param size How many bytes to write from source.
     * @return was data succesfully written.
     */
    bool write(const void* source, int size);

    /**
     * Get used local socket pointer.
     *
     * @return local socket or NULL if connection is closed or stolen.
     */
    QLocalSocket* getSocket() const;

    /**
     * Get used local socket pointer and steal ownership of it
     * from SessionData.
     *
     * @return local socket or NULL if connection is closed or stolen.
     */
    QLocalSocket* stealSocket();

    /**
     * Set used interval for the data stream. If data is received at higher
     * rate samples will be dropped.
     *
     * @param interval Interval in milliseconds.
     */
    void setInterval(int interval);

    /**
     * Get used interval for the data stream.
     *
     * @return used interval in milliseconds.
     */
    int getInterval() const;

    /**
     * Set buffer size for the data stream. This will set the window size for
     * how many data elements will be written in bursts. Setting buffer
     * size will disable the effect from set interval.
     *
     * @param size buffer size.
     */
    void setBufferSize(unsigned int size);

    /**
     * Get buffer size for the data stream.
     */
    unsigned int getBufferSize() const;

    /**
     * Set buffer interval for the data stream. This will configure the
     * timeout for how long will be waited for buffer to get full before
     * writing data to the socket.
     *
     * @param interval interval in milliseconds.
     */
    void setBufferInterval(unsigned int interval);

    /**
     * Get buffer inteval for the data stream.
     *
     * @return buffer interval in milliseconds.
     */
    unsigned int getBufferInterval() const;

    /**
     * Enable or disable downsampling. Downsampling is implemented by
     * just dropping extra samples.
     *
     * @param value enable or disable downsampling.
     */
    void setDownsampling(bool value);

    /**
     * Get downsampling state.
     *
     * @return is downsampling enabled.
     */
    bool getDownsampling() const;

private:
    /**
     * How many milliseconds since last time data was written to socket.
     *
     * @return How many milliseconds since last time data was
     *         written to socket.
     */
    long sinceLastWrite() const;

    /**
     * Write data directly to the socket.
     *
     * @param source Source from where to write.
     * @param size How many bytes to write.
     * @param count How many data elements are written.
     */
    bool write(void* source, int size, unsigned int count);

    /**
     * Delayed write invocation.
     *
     * @return was writing to socket succesful.
     */
    bool delayedWrite();

    QLocalSocket* socket;        /**< socket pointer. */
    int interval;                /**< interval in milliseconds. */
    char* buffer;                /**< pointer to buffer allocation. */
    int size;                    /**< allocated buffer size. */
    unsigned int count;          /**< how many elements are in the buffer */
    struct timeval lastWrite;    /**< when data was written last time */
    QTimer timer;                /**< timer for delayed write */
    unsigned int bufferSize;     /**< buffer size */
    unsigned int bufferInterval; /**< buffer interval in milliseconds */
    bool downsampling;           /**< sample dropping */

private slots:

    /**
     * Callback for delayed write timer.
     */
    void timerTimeout();
};

/**
 * Establishes and track session data connections.
 */
class SocketHandler : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SocketHandler)

public:
    /**
     * Constructor.
     *
     * @param parent Parent object.
     */
    SocketHandler(QObject* parent = 0);

    /**
     * Destructor.
     */
    ~SocketHandler();

    /**
     * Start to listen incoming connections.
     *
     * @param serverName Name to listen for connections.
     * @return was listening started succesfully.
     */
    bool listen(const QString& serverName);

    /**
     * Write data to given session.
     *
     * @param id Session ID.
     * @param source Location from where to write.
     * @param size How many bytes to write.
     */
    bool write(int id, const void* source, int size);

    /**
     * Close related socket connection for session.
     *
     * @param sessionId Session ID.
     * @return was socket connection closed succesfully.
     */
    bool removeSession(int sessionId);

    /**
     * Get socket file descriptor for given session.
     *
     * @param sessionId Session ID.
     * @return socket file descriptor.
     */
    int getSocketFd(int sessionId) const;

    /**
     * Set interval for given session. For more details see
     * #SessionData::setInterval(int).
     *
     * @param sessionId Session ID.
     * @param value Interval in milliseconds.
     */
    void setInterval(int sessionId, int value);

    /**
     * Remove set interval from given session.
     *
     * @param sessionId Session ID.
     */
    void clearInterval(int sessionId);

    /**
     * Get interval for given session. For more details see
     * #SessionData::interval().
     *
     * @param sessionId Session ID.
     * @return interval in milliseconds.
     */
    int interval(int sessionId) const;

    /**
     * Set buffer size for given session. For more details see
     * #SessionData::setBufferSize(unsigned int).
     *
     * @param sessionId Session ID.
     * @param value buffer size.
     */
    void setBufferSize(int sessionId, unsigned int value);

    /**
     * Remove set buffer size for given session.
     *
     * @param sessionId Session ID.
     */
    void clearBufferSize(int sessionId);

    /**
     * Get buffer size for given session. For more details see
     * #SessionData::bufferSize().
     *
     * @param sessionId Session ID.
     * @return buffer size.
     */
    unsigned int bufferSize(int sessionId) const;

    /**
     * Set buffer interval for given session. For more details see
     * #SessionData::setBufferInterval(unsigned int).
     *
     * @param sessionId Session ID.
     * @param value buffer inteval in milliseconds.
     */
    void setBufferInterval(int sessionId, unsigned int value);

    /**
     * Remove set buffer inteval for given session.
     *
     * @param sessionId Session ID.
     */
    void clearBufferInterval(int sessionId);

    /**
     * Get buffer interval for given session. For more details see
     * #SessionData::bufferInterval().
     *
     * @param sessionId Session ID.
     * @return interval in milliseconds.
     */
    unsigned int bufferInterval(int sessionId) const;

    /**
     * Is downsampling enabled for given session. For more details see
     * #SessionData::downsampling().
     *
     * @param sessionId Session ID.
     * @return is downsampling enabled.
     */
    bool downsampling(int sessionId) const;

    /**
     * Set downsampling for given session. For more details see
     * #SessionData::downsampling(bool).
     *
     * @param sessionId Session ID.
     * @param value downsampling state.
     */
    void setDownsampling(int sessionId, bool value);

Q_SIGNALS:
    /**
     * Signal is emitted for lost sessions which can happen for example
     * if application using sensorfw crashes.
     *
     * @param sessionId Session ID.
     */
    void lostSession(int sessionId);

private slots:
    /**
     * Callback for new client connection.
     */
    void newConnection();

    /**
     * Callback for new data in socket.
     */
    void socketReadable();

    /**
     * Callback for disconnected client.
     */
    void socketDisconnected();

    /**
     * Callback for socket error.
     *
     * @param socketError socket error.
     */
    void socketError(QLocalSocket::LocalSocketError socketError);

private:

    QLocalServer*            m_server; /**< listening server socket. */
    QMap<int, SessionData*>  m_idMap;  /**< map of client sessions. */
};

#endif // SOCKETHANDLER_H
