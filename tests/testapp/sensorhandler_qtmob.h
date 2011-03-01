/**
   @file sensorhandler_qmsystem2.h
   @brief test application to create sensor

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

#ifndef SENSORHANDLER_H
#define SENSORHANDLER_H

#include <QVector>
#include "qt-api/abstractsensor_i.h"
#include "datatypes/xyz.h"
#include "config.h"
#include <QThread>

class SensorHandler : public QThread
{
    Q_OBJECT
public:

    SensorHandler(const QString& sensorName, QObject *parent = 0);

    void startClient();

    int dataCount() const { return dataCount_; }
    QString sensorName() const { return sensorName_; }

    static void init(const QStringList& sensors);

private:
    QString sensorName_;
    int interval_;
    int bufferinterval_;
    bool standbyoverride_;
    int buffersize_;
    int dataCount_;
    int frameCount_;
};

#endif // SENSORHANDLER_H
