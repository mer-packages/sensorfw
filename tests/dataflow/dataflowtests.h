/**
   @file dataflowtests.h
   @brief Automatic tests for data flow
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Pia Niemela <pia.s.niemela@nokia.com>

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

#ifndef DATAFLOWTESTS_H
#define DATAFLOWTESTS_H

#include <QTest>
#include "sensormanager.h"

class DataFlowTest : public QObject
{
    Q_OBJECT;
public:
    DataFlowTest() : sm(0) {};

private:
    SensorManager* sm;

private slots:
    void initTestCase();
    void init() {}

    void testAdaptorSharing();
    void testChainSharing();

    void cleanup() {};
    void cleanupTestCase();
};

class TestSensorManager2: public SensorManager
{
public:
    static inline int getAdaptorCount(const SensorManager &that, QString adaptorName)
    {
        QList<QString> keys = static_cast<const TestSensorManager2*>(&that)->SensorManager::deviceAdaptorInstanceMap_.keys();
        int count = 0;
        foreach(QString key, keys) {
            if (static_cast<const TestSensorManager2*>(&that)->SensorManager::deviceAdaptorInstanceMap_[adaptorName].adaptor_) {
                count++;
            }
        }
        return count;
    }

    static inline QList<QString> getKeys(const SensorManager &that)
    {

        return static_cast<const TestSensorManager2*>(&that)->SensorManager::deviceAdaptorInstanceMap_.keys();
    }


    static inline int getRefCountSum(const SensorManager &that){
        QList<QString> keys = static_cast<const TestSensorManager2*>(&that)->SensorManager::deviceAdaptorInstanceMap_.keys();
        int refCount = 0;
        foreach(QString key, keys){
            refCount += TestSensorManager2::getRefCount(that,key);
        }
        return refCount;

    }

    static inline int getRefCount(const SensorManager &that, QString key){
        return ((DeviceAdaptorInstanceEntry)(static_cast<const TestSensorManager2*>(&that)->SensorManager::deviceAdaptorInstanceMap_).value(key)).cnt_;

    }

};


#endif // DATAFLOWTESTS_H
