/**
   @file metadatatest.h
   @brief Automatic tests for sensor client interfaces

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

#ifndef METADATA_TEST_H
#define METADATA_TEST_H

#include <QTest>

#include <QObject>
#include <QList>

class DummyHelper : public QObject
{
    Q_OBJECT;

public:
    bool hasReceived(const QString str)
    {
        return m_changedProperties.contains(str);
    }

public slots:
    void propertyChanged(const QString& str)
    {
        if (!m_changedProperties.contains(str)) {
            m_changedProperties << str;
        }
    }

private:
    QList<QString> m_changedProperties;
};


class MetaDataTest : public QObject
{
    Q_OBJECT;

private slots:
    // Setup tests
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void testDescription();

    void testChangeNotifications();

    void testAvailableRanges();
    void testGetCurrentRange();
    void testConcurrentRangeRequests();

    void testAvailableIntervals();
};

#endif // METADATA_TEST_H
