/**
   @file dummyclient.h
   @brief Helper client for automatic tests

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

#include <QObject>
#include "sensormanagerinterface.h"
#include "orientationsensor_i.h"

#ifndef DUMMYCLIENT_H
#define DUMMYCLIENT_H

class DummyClient : public QObject
{
    Q_OBJECT;
public:
    DummyClient(QObject* parent=0) : QObject(parent), sensor(NULL)
    {
        SensorManagerInterface& sm = SensorManagerInterface::instance();

        sm.loadPlugin("orientationsensor");
        sm.registerSensorInterface<OrientationSensorChannelInterface>("orientationsensor");

        sensor = OrientationSensorChannelInterface::interface("orientationsensor");
        if (sensor == NULL || !sensor->isValid()) {
            qDebug() << "[DummyClient] Unable to get session:" << sm.errorString();
        } else {
            sensor->start();
        }
    }

signals:
    void sessionClosed();

public slots:
    void closeSession() {
        if (sensor) {
            sensor->stop();
            delete sensor;
        }
        emit sessionClosed();
    }
    

private:
    OrientationSensorChannelInterface* sensor;
};

#endif
