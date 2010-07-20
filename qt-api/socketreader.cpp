/**
   @file socketreader.cpp
   @brief SocketReader helper class for sensor interface

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

#include "socketreader.h"

SocketReader::SocketReader(QObject* parent) :
    QObject(parent), socket_(NULL), tagRead_(false) {}

SocketReader::~SocketReader()
{
    if (socket_) {
        dropConnection();
    }
}

bool SocketReader::initiateConnection(int sessionId)
{
    if (socket_ != NULL) {
        qDebug() << "attempting to initiate connection on connected socket";
        return false;
    }

    socket_ = new QLocalSocket(this);
    socket_->connectToServer("/tmp/sensord.sock", QIODevice::ReadWrite);

    if (!(socket_->serverName().size())) {
        qDebug() << socket_->errorString();
        return false;
    }

    if (socket_->write((const char*)&sessionId, sizeof(sessionId)) != sizeof(sessionId)) {
        qDebug() << "[SOCKETREADER]: SessionId write failed:" << socket_->errorString();
    }
    socket_->flush();

    return true;
}

bool SocketReader::dropConnection()
{
    if (socket_ == NULL) {
        qDebug() << "Attempting to drop non-existant connection.";
        return false;
    }
    
    socket_->disconnectFromServer();
    delete socket_;
    socket_ = NULL;

    tagRead_ = false;

    return true;
}

QLocalSocket* SocketReader::socket()
{
    return socket_;
}

bool SocketReader::readSocketTag()
{
    if (!isConnected()) {
        qDebug() << "Not connected";
        return false;
    }
    
    QString idString(CHANNEL_ID_STRING);
    char tmpBuffer[idString.size()+1];
    
    memset(tmpBuffer, 0x0, idString.size()+1);
    if (idString.size()+1 > socket_->read(tmpBuffer, idString.size()+1)) {
        return false;
    }
    tagRead_ = true;

    return true;
}

bool SocketReader::read(void* buffer, int size)
{
    if (!socket_) {
        return false;
    }

    // TODO: Why must this be done here? Why does it not get the data
    //       if placed to AbstractSensorChannel::start() after sensor
    //       start..?
    if (!tagRead_) {
        if (!readSocketTag()) {
            // set error on failed tag read
        }
    }
    
    int bytesRead = socket_->read((char *)buffer, size);
    if (bytesRead <= 0) {
        return false;
    }
    return true;
}

bool SocketReader::isConnected()
{
    if (socket_ && socket_->isValid() && socket_->state() == QLocalSocket::ConnectedState) {
        return true;
    }
    return false;
}
