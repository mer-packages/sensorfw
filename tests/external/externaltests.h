/**
   @file externaltests.h
   @brief Automatic tests for external issues
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

#ifndef EXTERNALTEST_H
#define EXTERNALTEST_H

#include <QTest>

/**
 * Tests for external matters, such as GConf keys set by other frameworks
 * etc.
 */
class ExternalTest : public QObject
{
     Q_OBJECT;

private slots:
    void initTestCase() {}
    void init() {}

    void testDeclinationCorrectionKey();

    void cleanup() {}
    void cleanupTestCase() {}
};

#endif // EXTERNALTEST_H
