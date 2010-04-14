/**
   @file driverpolltest.cpp
   @brief A test for power management in driver poll/interrupt mode
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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

#include "driverpolltest.h"

#include <QtCore>
#include <QTimer>
#include <QDebug>

#include "sensormanagerinterface.h"
#include "accelerometersensor_i.h"
#include "datatypes/xyz.h"

#define MODE_SWITCH_INTERVAL 20000 // in ms, 20s

DriverPollTester::DriverPollTester(QObject *parent) : QObject(parent), poll(false)
{
    modeTimer = new QTimer(this);
    QObject::connect(modeTimer, SIGNAL(timeout()), this, SLOT(needModeSwitch()));
    modeTimer->start(MODE_SWITCH_INTERVAL);

    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
    if (!remoteSensorManager.isValid()) {
        qDebug() << "FATAL. Unable to init remote sensor manager.";
        return;
    }

    remoteSensorManager.loadPlugin("accelerometersensor");
    remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");

    // Start interfaces in polling mode.
    accOne = const_cast<AccelerometerSensorChannelInterface*>(AccelerometerSensorChannelInterface::listenInterface("accelerometersensor"));
    accTwo = const_cast<AccelerometerSensorChannelInterface*>(AccelerometerSensorChannelInterface::listenInterface("accelerometersensor"));

    if (!(accOne && accTwo)) {
        qDebug() << "FATAL. Unable to get accelerometer listen interfaces";
        return;
    }

    QObject::connect(accOne, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(slotDataAvailableA(const XYZ&)));
    QObject::connect(accTwo, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(slotDataAvailableB(const XYZ&)));

    qDebug() << "Sensor interfaces registered, starting in interrupt mode";
    switchMode(false);
}

DriverPollTester::~DriverPollTester()
{
    modeTimer->stop();
}

void DriverPollTester::switchMode(bool needStopping)
{
    if (needStopping) {
        accOne->stop();
        accTwo->stop();
    }

    if (poll) {
        accOne->setInterval(20);
        accTwo->setInterval(100);

        accOne->start();
        accTwo->start();
    } else {
        /* hw interrupt mode */
        accOne->setInterval(0); // 0 == hw interrupt
        accOne->start();
        // Don't start the second interface 
    } 
}

void DriverPollTester::needModeSwitch()
{
     poll = !poll;

     if (poll) {
         switchMode();
         qDebug() << "****** Mode switch: poll";
     } else {
         switchMode();
         qDebug() << "****** Mode switch: interrupt";
     }
}

void DriverPollTester::slotDataAvailableA(const XYZ& data)
{
     qDebug() << "acc1: " << data.x() << " " << data.y() << " " << data.z();
}

void DriverPollTester::slotDataAvailableB(const XYZ& data)
{
     qDebug() << "acc2: " << data.x() << " " << data.y() << " " << data.z();
}

int main(int argc, char **argv)
{
     QCoreApplication app(argc, argv);
     DriverPollTester pollTester;
     return app.exec();
}
