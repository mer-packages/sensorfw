/**
   @file sockethandler.h
   @brief SocketHandler

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QList>

class QLocalServer;
class QLocalSocket;

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

Q_SIGNALS:
    void lostSession(int sessionId);

private slots:
    void newConnection();
    void socketReadable();
    void socketDisconnected();
    void killSocket();

private:

    QLocalServer*            m_server;
    QMap<int, QLocalSocket*> m_idMap;
    QList<QLocalSocket*>     m_tmpSocks;
};

#endif // SOCKETHANDLER_H
