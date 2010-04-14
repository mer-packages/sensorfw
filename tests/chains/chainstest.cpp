/**
   @file chainstest.cpp
   @brief Automatic tests for sensor chains

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#include "chainstest.h"

#include <sensord/sensormanager.h>
#include <sensord/abstractchain.h>
#include <sensord/config.h>

void ChainTest::initTestCase()
{
    Config::loadConfig("/etc/sensord.conf");
}

void ChainTest::init() {}

void ChainTest::cleanup() {}

void ChainTest::cleanupTestCase() {}

void ChainTest::testCompassChain()
{
    SensorManager& sm = SensorManager::instance();
    QCOMPARE(sm.loadPlugin("compasschain"), true);

    AbstractChain* chain = sm.requestChain("compasschain");
    QVERIFY(chain);

    // Locate buffers
    QVERIFY(chain->findBuffer("magneticnorth"));
    QVERIFY(chain->findBuffer("truenorth"));

    QVERIFY(chain->start());
    QVERIFY(chain->stop());

    sm.releaseChain("compasschain");
}

void ChainTest::testAccelerometerChain()
{
    SensorManager& sm = SensorManager::instance();
    QCOMPARE(sm.loadPlugin("accelerometerchain"), true);

    AbstractChain* chain = sm.requestChain("accelerometerchain");
    QVERIFY(chain);

    // Locate buffers
    QVERIFY(chain->findBuffer("accelerometer"));

    QVERIFY(chain->start());
    QVERIFY(chain->stop());

    sm.releaseChain("accelerometerchain");
}

QTEST_MAIN(ChainTest)
