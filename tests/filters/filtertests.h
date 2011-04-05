/**
   @file filtertests.h
   @brief Automatic tests for filters
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

#ifndef FILTERAPITEST_H
#define FILTERAPITEST_H

#define CONFIG_FILE_PATH     "/etc/sensorfw/sensord.conf"
#define CONFIG_DIR_PATH      "/etc/sensorfw/sensord.conf.d/"

#include <QTest>
#include <typeinfo>
#include "pusher.h"
#include "dataemitter.h"
#include "source.h"
#include "orientationdata.h"
#include "posedata.h"

class FilterApiTest : public QObject
{
     Q_OBJECT

private slots:
    void initTestCase();
    void init() {}

    void testCoordinateAlignFilter();
    void testTopEdgeInterpretationFilter();
    void testFaceInterpretationFilter();
    void testDeclinationFilter();
    void testOrientationInterpretationFilter();
    void testRotationFilter();

    void cleanup() {}
    void cleanupTestCase() {}
};

/**
 * DummyAdaptor is a Pusher that can be used to push data into a filter for testing.
 * Input data is given as an array. Calling \c pushNewData() will propagate the next
 * value in the array into adaptor output.
 *
 * @todo For some reason we can only feed in 10 samples.. Anything beyond that will
 *       get compared to wrong expected output..
 */
template <class TYPE>
class DummyAdaptor : public Pusher
{
public:
    DummyAdaptor() : counter_(0), datacount_(0), data_(NULL), index_(0) {
        addSource(&source_, "source");
    };

    void setTestData(int count, TYPE *data) {
        datacount_ = count;
        data_ = data;
        index_ = 0;
    }

    void pushNewData() {
        if (index_ >= datacount_) {
            QVERIFY2(false, "Test function error: out of input data.");
            index_ = 0;
        }

        source_.propagate(1, &(data_[index_]));

        ++index_;
        ++counter_;
    }

    int getDataCount() { return counter_; }

private:
    Source<TYPE> source_;
    int counter_;
    int datacount_;
    TYPE *data_;
    int index_;
};

/**
 * DummyDataEmitter is a DataEmitter that can be used to capture output data of
 * a filter for testing purposes. The expected output data is given through a
 * function, and is compared to actual received output  in \c emitData() function.
 */
template <class TYPE>
class DummyDataEmitter : public DataEmitter<TYPE>
{
public:
    DummyDataEmitter() : DataEmitter<TYPE>(10),
                         samplesReceived_(0),
                         datacount_(0),
                         data_(NULL),
                         index_(0) {}

    void setExpectedData(int count, TYPE *data) {
        datacount_ = count;
        data_ = data;
        index_ = 0;
    }

    int numSamplesReceived() { return samplesReceived_; }

    void emitData(const TYPE& data) {
        int i = index_;
        samplesReceived_++;
        index_++;
        if (index_ > datacount_) {
            QVERIFY2(false, "Test function error: out of expected results.");
            index_ = 0;
        }

        // Check type
        if (typeid(TYPE) == typeid(PoseData)) {
            PoseData *d1 = (PoseData *)&data;
            PoseData *d2 = (PoseData *)&(data_[i]);
            QCOMPARE(d1->timestamp_, d2->timestamp_);
            QCOMPARE(d1->orientation_, d2->orientation_);

        } else if (typeid(TYPE) == typeid(TimedUnsigned)) {
            TimedUnsigned *d1 = (TimedUnsigned *)&data;
            TimedUnsigned *d2 = (TimedUnsigned *)&(data_[i]);
            QCOMPARE(d1->timestamp_, d2->timestamp_);
            QCOMPARE(d1->value_, d2->value_);

        } else if (typeid(TYPE) == typeid(TimedXyzData)) {
            TimedXyzData *d1 = (TimedXyzData *)&data;
            TimedXyzData *d2 = (TimedXyzData *)&(data_[i]);
            QCOMPARE(d1->timestamp_, d2->timestamp_);
            QCOMPARE(d1->x_, d2->x_);
            QCOMPARE(d1->y_, d2->y_);
            QCOMPARE(d1->z_, d2->z_);

        } else if (typeid(TYPE) == typeid(CompassData)) {
            CompassData *d1 = (CompassData *)&data;
            CompassData *d2 = (CompassData *)&(data_[i]);
            QCOMPARE(d1->timestamp_, d2->timestamp_);
            QCOMPARE(d1->correctedDegrees_, d2->correctedDegrees_);
            QCOMPARE(d1->degrees_, d2->degrees_);
            QCOMPARE(d1->level_, d2->level_);

        } else {
            QWARN("No comparison method for this type");
        }
    }

private:
    int samplesReceived_;
    int datacount_;
    TYPE *data_;
    int index_;
};


#endif // FILTERAPITEST_H
