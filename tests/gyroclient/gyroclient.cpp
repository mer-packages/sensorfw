/**
   @file gyroclient.cpp
   @brief Manual test for gyroscope client interfaces

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>

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

#include <QtCore>

#include "qt-api/sensormanagerinterface.h"
#include "gyroclient.h"

GyroClient::GyroClient()
{
    QDBusReply<void> reply;
    QString sensorName("gyroscopesensor");

    SensorManagerInterface& sm = SensorManagerInterface::instance();
    Q_ASSERT( sm.isValid() );

    sm.loadPlugin("gyroscopesensor");
    sm.registerSensorInterface<GyroscopeSensorChannelInterface>("gyroscopesensor");

    m_sensorIfc = GyroscopeSensorChannelInterface::controlInterface(sensorName);
    Q_ASSERT (m_sensorIfc && m_sensorIfc->isValid());
    reply = m_sensorIfc->start();
    Q_ASSERT (reply.isValid());

    if (!(QObject::connect(m_sensorIfc, SIGNAL(dataAvailable(const XYZ&)),
                           this, SLOT(slotDataAvailable(const XYZ&)))))
        qWarning() << "Unable to connect" << sensorName;
}

GyroClient::~GyroClient()
{
    delete m_sensorIfc;
}

void GyroClient::slotDataAvailable(const XYZ& data)
{
    qDebug() << "raw gyro data: (" << data.x() << data.y() << data.z() << ")";
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    GyroClient gyro;
    return app.exec();
}
