/**
   @file socketreader.h
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

#ifndef SOCKETREADER_H
#define SOCKETREADER_H

#include <QObject>
#include <QLocalSocket>
#include <QVector>

/**
 * @brief Helper class for reading socket datachannel from sensord
 *
 * SocketReader provides common handler for all sensors using socket
 * data channel. It is used by AbstractSensorChannelInterface to maintain
 * the socket connection to the server.
 */
class SocketReader : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SocketReader)

public:
    static const char* channelIDString;

    SocketReader(QObject* parent = 0);
    ~SocketReader();

    /**
     * Initiates socket connection.
     * @param sessionId ID for the current session. Must be unique to map
     *                  connection to session properly.
     * @return \c true when new connection is succesfully created.
     *         \c false otherwise.
     */
    bool initiateConnection(int sessionId);

    /**
     * Drops socket connection.
     * @return \c true when connection is succesfully dropped. \c false
     *         otherwise.
     */
    bool dropConnection();

    /**
     * Provides access to the internal QLocalSocket for direct reading.
     * @return Pointer to the internal QLocalSocket. Pointer can be \c NULL
     *         if \c initiateConnection() has not been called.
     */
    QLocalSocket* socket();

    /**
     * Attempt to read given number of bytes from the socket. As
     * QLocalSocket is used, we are guaranteed that any number of bytes
     * written in single operation are available for immediate reading
     * with a single operation.
     *
     * @param unitSize Number of bytes to read.
     * @param buffer   Location for storing the data.
     * @return \c true if any bytes were read, \c false otherwise.
     */
    bool read(void* buffer, int size);

    /**
     * Attempt to read all objects which are being written to the socket.
     *
     * @param values   QList to which objects will be appended.
     * @return \c true if any bytes were read, \c false otherwise.
     */
    template<typename T>
    bool read(QVector<T>& values);

    /**
     * Returns whether the socket is currently connected.
     * @return \c true if connected, \c false if not.
     */
    bool isConnected();

private:
    bool readSocketTag();

    QLocalSocket* socket_;
    bool tagRead_;
};

template<typename T>
bool SocketReader::read(QVector<T>& values)
{
    unsigned int count;
    if(!read((void*)&count, sizeof(unsigned int)))
        return false;
    values.reserve(values.size() + count);
    for(unsigned int i = 0; i < count; ++i)
    {
        T value;
        if(!read((void*)&value, sizeof(T)))
            return false;
        values.push_back(value);
    }
    return true;
}

#endif // SOCKETREADER_H
