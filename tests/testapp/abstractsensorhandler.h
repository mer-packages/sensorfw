/**
   @file abstractsensorhandler.h
   @brief base class for sensorhandlers

   <p>
   Copyright (C) 2011 Nokia Corporation

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

#ifndef ABSTRACTSENSORHANDLER_H
#define ABSTRACTSENSORHANDLER_H

#include <QThread>
#include <QString>
#include "qt-api/abstractsensor_i.h"

Q_DECLARE_METATYPE( QAbstractSocket::SocketState)

class AbstractSensorHandler : public QThread
{
public:
    AbstractSensorHandler(const QString& sensorName, QObject *parent = 0);

    virtual ~AbstractSensorHandler();

    virtual void run();
    virtual bool startClient() = 0;
    virtual bool stopClient() = 0;

    int dataCount() const { return dataCount_; }
    QString sensorName() const { return sensorName_; }

protected:
    QString sensorName_;
    int interval_;
    int bufferinterval_;
    bool standbyoverride_;
    int buffersize_;
    int dataCount_;
    int frameCount_;
};

#endif // ABSTRACTSENSORHANDLER_H
