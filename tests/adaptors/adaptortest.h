/**
   @file adaptortest.h
   @brief Automatic tests for adaptors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

#ifndef ADAPTORTEST_H
#define ADAPTORTEST_H

#include <QTest>

class AdaptorTest : public QObject
{
     Q_OBJECT;

public:
    AdaptorTest()
    {
    }

protected:

private slots:

    // Setup
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    // Adaptor functionality
    void testAccelerometerAdaptor();
    void testMagnetometerAdaptor();
    void testALSAdaptor();
    void testTapAdaptor();
    void testKeyboardSliderAdaptor();
    void testProximityAdaptor();
    void testTouchAdaptor();

};

#endif // ADAPTORTEST_H
