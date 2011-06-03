/**
   @file standbyoverridetests.h
   @brief Automatic tests for standby override
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
   
   This file is part of Sensord.

   Sensord is free software; youcan redistribute it and/or modify
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

#ifndef SENSORPM_TEST_H
#define SENSORPM_TEST_H

#include <QTest>
#include "helpslot.h"

#define accPollFile "/tmp/fakedsensors/accelerometer_poll_rate"
#define accInputFile "/tmp/fakedsensors/accelerometer"


class StandbyOverrideTest : public QObject
{
    Q_OBJECT;

private slots:
    // Setup tests
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    // Tests Standby override when screen gets blank 
    void testStandbyOverride();

private:
    HelpSlot helper1, helper2;
};

#endif // SENSORPM_TEST_H
