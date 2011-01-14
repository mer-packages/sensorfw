/**
   @file sockethandler.cpp
   @brief SocketHandler

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#include <QLocalSocket>
#include <QLocalServer>
#include <QMutexLocker>
#include <sys/socket.h>
#include "logging.h"
#include "sockethandler.h"
#include <unistd.h>
#include <limits.h>

SessionData::SessionData(QLocalSocket* socket, QObject* parent) : QObject(parent),
                                                                  socket(socket),
                                                                  interval(-1),
                                                                  buffer(0),
                                                                  size(0),
                                                                  count(0),
                                                                  bufferSize(1),
                                                                  bufferInterval(0),
                                                                  mutex(QMutex::Recursive)
{
    lastWrite.tv_sec = 0;
    lastWrite.tv_usec = 0;
    connect(&timer, SIGNAL(timeout()), this, SLOT(delayedWrite()));
}

SessionData::~SessionData()
{
    timer.stop();
    delete socket;
    delete[] buffer;
}

long SessionData::sinceLastWrite() const
{
    if(lastWrite.tv_sec == 0)
        return LONG_MAX;
    struct timeval now;
    gettimeofday(&now, 0);
    return (now.tv_sec - lastWrite.tv_sec) * 1000 + ((now.tv_usec - lastWrite.tv_usec) / 1000);
}

bool SessionData::write(void* source, int size, unsigned int count)
{
    if(socket && count)
    {
        sensordLogT() << "[SocketHandler]: writing " << count << " fragments to socket with payload (bytes): " << size;
        memcpy(source, &count, sizeof(unsigned int));
        int written = socket->write((const char*)source, size * count + sizeof(unsigned int));
        if(written < 0)
        {
            sensordLogW() << "[SocketHandler]: failed to write payload to the socket: " << socket->errorString();
            return false;
        }
        return true;
    }
    return false;
}

bool SessionData::write(const void* source, int size)
{
    QMutexLocker locker(&mutex);
    long since = sinceLastWrite();
    int allocSize = bufferSize * size + sizeof(unsigned int);
    if(!buffer)
        buffer = new char[allocSize];
    else if(size != this->size)
    {
        if(buffer)
        {
            socket->waitForBytesWritten();
            delete[] buffer;
        }
        buffer = new char[allocSize];
    }
    this->size = size;
    if(bufferSize <= 1)
    {
        memcpy(buffer + sizeof(unsigned int), source, size);
        if(since >= interval)
        {
            sensordLogT() << "[SocketHandler]: writing, since > interval";
            gettimeofday(&lastWrite, 0);
            return write(buffer, size, 1);
        }
    }
    else
    {
        memcpy(buffer + sizeof(unsigned int) + size * count, source, size);
        ++count;
        if(bufferSize == count)
            delayedWrite();
    }
    if(!timer.isActive())
    {
        if(bufferSize > 1 && bufferInterval)
        {
            sensordLogT() << "[SocketHandler]: delayed write by " << bufferInterval << "ms";
            timer.start(bufferInterval);
        }
        else if((interval - since) > 0)
        {
            sensordLogT() << "[SocketHandler]: delayed write by " << (interval - since) << "ms";
            timer.start(interval - since);
        }
        else
            delayedWrite();
    }
    else
    {
        sensordLogT() << "[SocketHandler]: timer already running";
    }
    return true;
}

void SessionData::delayedWrite()
{
    QMutexLocker locker(&mutex);
    if(timer.isActive())
        timer.stop();
    gettimeofday(&lastWrite, 0);
    write(buffer, size, count);
    count = 0;
}

QLocalSocket* SessionData::stealSocket()
{
    QLocalSocket* tmpsocket = socket;
    socket = 0;
    return tmpsocket;
}

void SessionData::setInterval(int interval)
{
    QMutexLocker locker(&mutex);
    this->interval = interval;
}

int SessionData::getInterval() const
{
    return interval;
}

void SessionData::setBufferInterval(unsigned int interval)
{
    QMutexLocker locker(&mutex);
    bufferInterval = interval;
}

unsigned int SessionData::getBufferInterval() const
{
    return bufferInterval;
}

void SessionData::setBufferSize(unsigned int size)
{
    if(size != bufferSize)
    {
        QMutexLocker locker(&mutex);
        if(timer.isActive())
            timer.stop();
        socket->waitForBytesWritten();
        delete[] buffer;
        buffer = 0;
        count = 0;
        bufferSize = size;
        if(bufferSize < 1)
            bufferSize = 1;
        sensordLogT() << "[SocketHandler]: new buffersize: " << bufferSize;
    }
}

unsigned int SessionData::getBufferSize() const
{
    return bufferSize;
}

SocketHandler::SocketHandler(QObject* parent) : QObject(parent), m_server(NULL)
{
    m_server = new QLocalServer(this);
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

SocketHandler::~SocketHandler()
{
    if (m_server) {
        delete m_server;
    }
}

bool SocketHandler::listen(QString serverName)
{
    if (m_server->isListening()) {
        sensordLogW() << "[SocketHandler]: Already listening";
        return false;
    }

    bool unlinkDone = false;
    while (!m_server->listen(serverName) && !unlinkDone && serverName[0] == QChar('/'))
    {
        if ( unlink(serverName.toLocal8Bit().constData()) == 0) {
            sensordLogD() << "[SocketHandler]: Unlinked stale socket" << serverName;
        } else {
            sensordLogD() << m_server->errorString();
        }
        unlinkDone = true;
    }
    return m_server->isListening();
}

bool SocketHandler::write(int id, const void* source, int size)
{
    // TODO: Calculate failed writes (some are expected if sockets initialize too slow)
    QMap<int, SessionData*>::iterator it = m_idMap.find(id);
    if (it == m_idMap.end())
    {
        sensordLogD() << "[SocketHandler]: Trying to write to nonexistent session (normal, no panic).";
        return false;
    }
    sensordLogT() << "[SocketHandler]: Writing to session " << id;
    return (*it)->write(source, size);
}

bool SocketHandler::removeSession(int sessionId)
{
    if (!(m_idMap.keys().contains(sessionId))) {
        sensordLogD() << "[SocketHandler]: Trying to remove nonexistent session.";
        return false;
    }

    QLocalSocket* socket = (*m_idMap.find(sessionId))->stealSocket();

    if (socket) {
        disconnect(socket, SIGNAL(readyRead()), this, SLOT(socketReadable()));
        disconnect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
        socket->deleteLater();
    }

    delete m_idMap.take(sessionId);

    return true;
}

void SocketHandler::newConnection()
{
    sensordLogT() << "[SocketHandler]: New connection received.";

    while (m_server->hasPendingConnections()) {

        QLocalSocket* socket = m_server->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadable()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));

        /// Do an initial write to instantiate the QObject child (why
        /// this happens in the write operation and not above is a
        /// mystery to me). This chunk provide info about datatype..
        // TODO: Might not need this now that we first receive a write from the other end.
        socket->write("_SENSORCHANNEL_", 16);
        socket->flush();
    }
}

void SocketHandler::socketReadable()
{
    int sessionId = -1;
    QLocalSocket* socket = (QLocalSocket*)sender();
    ((QLocalSocket*)sender())->read((char*)&sessionId, sizeof(int));

    disconnect(socket, SIGNAL(readyRead()), this, SLOT(socketReadable()));

    if (sessionId >= 0) {
        if(!m_idMap.contains(sessionId))
            m_idMap.insert(sessionId, new SessionData((QLocalSocket*)sender(), this));
    } else {
        // TODO: Handle in a clean way, don't die.
        sensordLogC() << "[SocketHandler]: Failed to read valid session ID from client.";
        Q_ASSERT(false);
    }
}

void SocketHandler::socketDisconnected()
{
    QLocalSocket* socket = (QLocalSocket*)sender();

    int sessionId = -1;
    for(QMap<int, SessionData*>::const_iterator it = m_idMap.begin(); it != m_idMap.end(); ++it)
    {
        if(it.value()->getSocket() == socket)
            sessionId = it.key();
    }

    if (sessionId == -1) {
        sensordLogW() << "[SocketHandler]: Noticed lost session, but can't find it.";
        return;
    }

    emit lostSession(sessionId);
}

int SocketHandler::getSocketFd(int sessionId) const
{
    QMap<int, SessionData*>::const_iterator it = m_idMap.find(sessionId);
    if (it != m_idMap.end() && (*it)->getSocket())
        return (*it)->getSocket()->socketDescriptor();
    return 0;
}

void SocketHandler::setInterval(int sessionId, int value)
{
    QMap<int, SessionData*>::iterator it = m_idMap.find(sessionId);
    if (it != m_idMap.end())
        (*it)->setInterval(value);
}

void SocketHandler::clearInterval(int sessionId)
{
    QMap<int, SessionData*>::iterator it = m_idMap.find(sessionId);
    if (it != m_idMap.end())
        (*it)->setInterval(-1);
}

int SocketHandler::interval(int sessionId) const
{
    QMap<int, SessionData*>::const_iterator it = m_idMap.find(sessionId);
    if (it != m_idMap.end())
        return (*it)->getInterval();
    return 0;
}

void SocketHandler::setBufferSize(int sessionId, unsigned int value)
{
    QMap<int, SessionData*>::iterator it = m_idMap.find(sessionId);
    if (it != m_idMap.end())
        (*it)->setBufferSize(value);
}

void SocketHandler::clearBufferSize(int sessionId)
{
    setBufferSize(sessionId, 0);
}

unsigned int SocketHandler::bufferSize(int sessionId) const
{
    QMap<int, SessionData*>::const_iterator it = m_idMap.find(sessionId);
    if (it != m_idMap.end())
        return (*it)->getBufferSize();
    return 0;
}

void SocketHandler::setBufferInterval(int sessionId, unsigned int value)
{
    QMap<int, SessionData*>::iterator it = m_idMap.find(sessionId);
    if (it != m_idMap.end())
        (*it)->setBufferInterval(value);
}

void SocketHandler::clearBufferInterval(int sessionId)
{
    setBufferInterval(sessionId, 0);
}

unsigned int SocketHandler::bufferInterval(int sessionId) const
{
    QMap<int, SessionData*>::const_iterator it = m_idMap.find(sessionId);
    if (it != m_idMap.end())
        return (*it)->getBufferInterval();
    return 0;
}
