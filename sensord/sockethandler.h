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
#include <sys/time.h>

class QLocalServer;
class QLocalSocket;

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
    void setBufferSize(int size);
    void setBufferInterval(int size);

private:
    long sinceLastWrite() const;
    bool write(const void* source, int size, int count);

    QLocalSocket* socket;
    int interval;
    char* buffer;
    int size;
    int count;
    struct timeval lastWrite;
    QTimer timer;
    int bufferSize;
    int bufferInterval;

private slots:
    void delayedWrite();
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

    void setBufferSize(int sessionId, int value);
    void clearBufferSize(int sessionId);

    void setBufferInterval(int sessionId, int value);
    void clearBufferInterval(int sessionId);

Q_SIGNALS:
    void lostSession(int sessionId);

private slots:
    void newConnection();
    void socketReadable();
    void socketDisconnected();
    void killSocket();

private:

    QLocalServer*            m_server;
    QMap<int, SessionData*>  m_idMap;
    QList<QLocalSocket*>     m_tmpSocks;
};

#endif // SOCKETHANDLER_H
