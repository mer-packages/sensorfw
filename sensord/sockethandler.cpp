/**
   @file sockethandler.cpp
   @brief SocketHandler

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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
#include <sys/socket.h>
#include "logging.h"
#include "sockethandler.h"
#include <unistd.h>

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
            qDebug() << m_server->errorString();
        }
        unlinkDone = true;
    }
    return m_server->isListening();
}

bool SocketHandler::write(int id, const void* source, int size)
{
    // TODO: Calculate failed writes (some are expected if sockets initialize too slow)
    if (!(m_idMap.contains(id))) {
        sensordLogD() << "[SocketHandler]: Trying to write to nonexistent session (normal, no panic).";
        return false;
    }

    QLocalSocket* socket = m_idMap.value(id);

    if (socket == NULL) {
        sensordLogW() << "[SocketHandler]: writing to null socket, socket conn was too slow probably.";
        return false;
    }

    int bytesWritten = socket->write((const char*)source, size);

    if ( bytesWritten < 0) {
        return false;
    }

    return true;
}

bool SocketHandler::removeSession(int sessionId)
{
    if (!(m_idMap.keys().contains(sessionId))) {
        sensordLogW() << "[SocketHandler]: Trying to remove nonexistent session.";
    }
    
    QLocalSocket* socket = m_idMap.value(sessionId);

    if (socket) {
        disconnect(socket, SIGNAL(readyRead()), this, SLOT(socketReadable()));
        disconnect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));

        // NOTE NOTE NOTE NOTE NOTE NOTE KLUDGE KLUDGE
        // Due to some timing issues that we have not been able to figure out,
        // deleting the socket right away does not work if the session is closed
        // after a client has been lost. Thus, socket deletion is pushed forward
        // 2 seconds to allow all Qt internal async stuff to finish..
        //delete socket;
        // NOTE NOTE NOTE NOTE NOTE NOTE KLUDGE KLUDGE
        m_tmpSocks.append(socket);
        QTimer::singleShot(2000, this, SLOT(killSocket()));
    }

    m_idMap.remove(sessionId);

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
        m_idMap.insert(sessionId, (QLocalSocket*)sender());
    } else {
        // TODO: Handle in a clean way, don't die.
        sensordLogC() << "[SocketHandler]: Failed to read valid session ID from client.";
        Q_ASSERT(false);
    }
}

void SocketHandler::socketDisconnected()
{
    QLocalSocket* socket = (QLocalSocket*)sender();

    int sessionId = m_idMap.key(socket);
    if (socket == 0) {
        sensordLogW() << "[SocketHandler]: Noticed lost session, but can't find it.";
        return;
    }

    emit lostSession(sessionId);
}

void SocketHandler::killSocket()
{
    if (m_tmpSocks.size() > 0) {
        sensordLogT() << "[SocketHandler]: Deleting socket pointer:" << m_tmpSocks.at(0);
        delete m_tmpSocks.takeAt(0);
    } else {
        sensordLogW() << "[SocketHandler]: Ugly hack just went bad.. attempting to delete nonexisting pointer.";
    }
}
