/**
   @file driverpolltest.h
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

#ifndef SENSORPM_DRIVERPOLLTEST_H
#define SENSORPM_DRIVERPOLLTEST_H

#include <QObject>

class QTimer;
class AccelerometerSensorChannelInterface;
class XYZ;

class DriverPollTester : public QObject {
    Q_OBJECT

public:
    DriverPollTester(QObject *parent = 0);
    virtual ~DriverPollTester();

private:
    // Timer switches between the poll and interrupt mode
    QTimer *modeTimer;
    bool poll;
    int execTimes;

    AccelerometerSensorChannelInterface *accOne;
    AccelerometerSensorChannelInterface *accTwo;

    void switchMode(bool needStopping = true);

private slots:
    void needModeSwitch();
    void slotDataAvailableA(const XYZ& data);
    void slotDataAvailableB(const XYZ& data);
};

#endif
