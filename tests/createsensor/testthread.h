/**
   @file testthread.h
   @brief test application to create sensor

   <p>
   Copyright (C) 2010-2011 Nokia Corporation

   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>

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

#ifndef TESTTHREAD_H
#define TESTTHREAD_H

#include <QThread>
#include <QVector>
#include "qt-api/abstractsensor_i.h"
#include "datatypes/xyz.h"


class Testthread : public QThread
{
    Q_OBJECT
public:

    Testthread(QString sensorName, QObject *parent = 0);
    Testthread(QObject *parent = 0);

    void setInterface(AbstractSensorChannelInterface* inf);
    void setInterval(int value);
    void setBufferInterval(int value);

    void run();

public Q_SLOTS:

    void receivedData(XYZ& data);
    void receivedFrame(QVector<XYZ>& frame);

private:
    QString sensorName;
    AbstractSensorChannelInterface* sensorChannelInterface;
    int interval;
    int bufferinterval;
    int dataCount;
    int frameCount;
};

#endif // TESTTHREAD_H
