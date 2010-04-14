/**
   @file chainstest.h
   @brief Automatic tests for sensor chains

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

#ifndef CHAINTEST_H
#define CHAINTEST_H

#include <QTest>

class ChainTest : public QObject
{
    Q_OBJECT;

public:
    ChainTest() {}

private slots:
    // Setup
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    // Chains
    void testCompassChain();
    void testAccelerometerChain();
};

#endif // CHAINTEST_H
