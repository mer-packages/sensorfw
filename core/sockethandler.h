/**
   @file sockethandler.h
   @brief SocketHandler

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

class SessionData : public QObject
{
    Q_OBJECT;
    Q_DISABLE_COPY(SessionData);

public:
    SessionData(QLocalSocket* socket, QObject* parent = 0);
    virtual ~SessionData();

    bool write(const void* source, int size);

    QLocalSocket* getSocket() const { return socket; };
    QLocalSocket* stealSocket();
    void setInterval(int interval);
    int getInterval() const;
    void setBufferSize(unsigned int size);
    unsigned int getBufferSize() const;
    void setBufferInterval(unsigned int interval);
    unsigned int getBufferInterval() const;

private:
    long sinceLastWrite() const;
    bool write(void* source, int size, unsigned int count);
    bool delayedWrite();

    QLocalSocket* socket;
    int interval;
    char* buffer;
    int size;
    unsigned int count;
    struct timeval lastWrite;
    QTimer timer;
    unsigned int bufferSize;
    unsigned int bufferInterval;
    QMutex mutex;

private slots:
    void timerTimeout();
};

/**
 *
 */
class SocketHandler : public QObject
{
    Q_OBJECT;

public:
    SocketHandler(QObject* parent = 0);
    ~SocketHandler();

    bool listen(QString serverName);
    bool write(int id, const void* source, int size);

    bool removeSession(int sessionId);
    int getSocketFd(int sessionId) const;

    void setInterval(int sessionId, int value);
    void clearInterval(int sessionId);
    int interval(int sessionId) const;

    void setBufferSize(int sessionId, unsigned int value);
    void clearBufferSize(int sessionId);
    unsigned int bufferSize(int sessionId) const;

    void setBufferInterval(int sessionId, unsigned int value);
    void clearBufferInterval(int sessionId);
    unsigned int bufferInterval(int sessionId) const;

Q_SIGNALS:
    void lostSession(int sessionId);

private slots:
    void newConnection();
    void socketReadable();
    void socketDisconnected();
    void socketError(QLocalSocket::LocalSocketError socketError);

private:

    QLocalServer*            m_server;
    QMap<int, SessionData*>  m_idMap;
};

#endif // SOCKETHANDLER_H
