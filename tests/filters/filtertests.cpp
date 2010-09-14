/**
   @file filtertests.cpp
   @brief Automatic tests for filters
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>

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

#include <QtDebug>
#include <QTest>
#include <QVariant>

#include <typeinfo>

#include <sensord/sensormanager.h>
#include "sensord/bin.h"
#include "sensord/bufferreader.h"
#include "sensord/filter.h"
#include "sensord/dbusemitter.h"
#include "coordinatealignfilter.h"
#include "orientationdata.h"
#include "orientationinterpreter.h"
#include "declinationfilter.h"
#include "rotationfilter.h"
#include "filtertests.h"
#include "timedunsigned.h"

#define WAIT_ROUND_DELAY 100
#define MAX_WAIT_ROUNDS 5
#define MAX_FACE_WAIT_ROUNDS 30

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
    DummyAdaptor() : counter_(0), datacount_(0), data_(NULL), loop_(false), index_(0) {
        addSource(&source_, "source");
    };

    void setTestData(int count, TYPE *data, bool loop) {
        datacount_ = count;
        loop_ = loop;
        data_ = data;
        index_ = 0;
    }

    void pushNewData() {
        if (index_ >= datacount_) {
            QVERIFY2(loop_, "Test function error: out of input data.");
            index_ = 0;
        }

        source_.propagate(1, &(data_[index_]));

        index_++;
        counter_++;
    }

    int getDataCount() { return counter_; }

private:
    Source<TYPE> source_;
    int counter_;
    int datacount_;
    TYPE *data_;
    bool loop_;
    int index_;
};



/**
 * DummyDbusEmitter is a DbusEmitter that can be used to capture output data of
 * a filter for testing purposes. The expected output data is given through a
 * function, and is compared to actual received output  in \c emitToDbus() function.
 */
template <class TYPE>
class DummyDbusEmitter : public DbusEmitter<TYPE> {
public:
    DummyDbusEmitter() : DbusEmitter<TYPE>(10),
                         samplesReceived_(0),
                         datacount_(0),
                         data_(NULL),
                         loop_(false),
                         index_(0) {}

    void setExpectedData(int count, TYPE *data, bool loop) {
        datacount_ = count;
        data_ = data;
        loop_ = loop;
        index_ = 0;
    }
    int numSamplesReceived() { return samplesReceived_; }

    void emitToDbus(const TYPE& data) {
        index_++;
        samplesReceived_++;

        if (index_ > datacount_) {
            QVERIFY2(loop_, "Test function error: out of expected results.");
            index_ = 0;
        }

        // Check type
        if (typeid(TYPE) == typeid(PoseData)) {
            PoseData *d1 = (PoseData *)&data;
            PoseData *d2 = (PoseData *)&(data_[index_-1]);
            QCOMPARE(d1->timestamp_, d2->timestamp_);
            QCOMPARE(d1->orientation_, d2->orientation_);

        } else if (typeid(TYPE) == typeid(TimedUnsigned)) {
            TimedUnsigned *d1 = (TimedUnsigned *)&data;
            TimedUnsigned *d2 = (TimedUnsigned *)&(data_[index_-1]);
            QCOMPARE(d1->timestamp_, d2->timestamp_);
            QCOMPARE(d1->value_, d2->value_);

        } else if (typeid(TYPE) == typeid(TimedXyzData)) {
            TimedXyzData *d1 = (TimedXyzData *)&data;
            TimedXyzData *d2 = (TimedXyzData *)&(data_[index_-1]);
            QCOMPARE(d1->timestamp_, d2->timestamp_);
            QCOMPARE(d1->x_, d2->x_);
            QCOMPARE(d1->y_, d2->y_);
            QCOMPARE(d1->z_, d2->z_);

        } else if (typeid(TYPE) == typeid(CompassData)) {
            CompassData *d1 = (CompassData *)&data;
            CompassData *d2 = (CompassData *)&(data_[index_-1]);
            QCOMPARE(d1->timestamp_, d2->timestamp_);
            QCOMPARE(d1->degrees_, d2->degrees_);
            QCOMPARE(d1->level_, d2->level_);

        } else {
            /*
             TYPE *d1 = &data;
             TYPE *d2 = &data_[index_-1];
             QCOMPARE(*d1, *d2);
             */
            //TODO: Figure out how to compare types that have '==' operator so that
            //      compiler will accept it.
            QWARN("No comparison method for this type");
        }
    }

protected:
    int samplesReceived_;
    int datacount_;
    TYPE *data_;
    bool loop_;
    int index_;
};

/**
 * This should be simple enough to be valid if one transformation matrix works.
 * Could add larger/smaller coefficients and another matrix if really pedant.
 */
void FilterApiTest::testCoordinateAlignFilter()
{
    // Transformation matrix to use for testing.
    double hconv[3][3] = {
        { 0, 0,-1},
        {-1, 0, 0},
        { 0, 1, 0}
    };

    // Input data to feed to the filter
    TimedXyzData inputData[] = {
        TimedXyzData(0, 0, 0, 0),
        TimedXyzData(0, 1, 0, 0),
        TimedXyzData(0, 0, 2, 0),
        TimedXyzData(0, 0, 0, 3),
        TimedXyzData(0, 4, 5, 0),
        TimedXyzData(0, 6, 0, 7),
        TimedXyzData(0, 0, 8, 9),
        TimedXyzData(0, 1, 1, 1)
    };

    // Expected output data
    TimedXyzData expectedResult[] = {
        TimedXyzData(0, 0, 0, 0),
        TimedXyzData(0, 0,-1, 0),
        TimedXyzData(0, 0, 0, 2),
        TimedXyzData(0,-3, 0, 0),
        TimedXyzData(0, 0,-4, 5),
        TimedXyzData(0,-7,-6, 0),
        TimedXyzData(0,-9, 0, 8),
        TimedXyzData(0,-1,-1, 1)
    };

    QVERIFY2((sizeof(inputData))==(sizeof(expectedResult)),
             "Test function error: Input and output count does not match.");

    int numInputs = (sizeof(inputData)/sizeof(TimedXyzData));

    Bin filterBin;
    DummyAdaptor<TimedXyzData> dummyAdaptor;

    FilterBase* coordAlignFilter = CoordinateAlignFilter::factoryMethod();

    ((CoordinateAlignFilter*)coordAlignFilter)->setProperty("transMatrix", QVariant::fromValue(TMatrix(hconv)));

    RingBuffer<TimedXyzData> outputBuffer(10);
    filterBin.add(&dummyAdaptor, "adapter");
    filterBin.add(coordAlignFilter, "coordfilter");
    filterBin.add(&outputBuffer, "buffer");

    filterBin.join("adapter", "source", "coordfilter", "sink");
    filterBin.join("coordfilter", "source", "buffer", "sink");

    DummyDbusEmitter<TimedXyzData> dbusEmitter;
    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdbusemitter");
    outputBuffer.join(&dbusEmitter);

    /* Setup data */
    dummyAdaptor.setTestData(numInputs, inputData, false);
    dbusEmitter.setExpectedData(numInputs, expectedResult, false);

    marshallingBin.start();
    filterBin.start();

    // Start sends data once, so start from index 1.
    for (int i=1; i < numInputs; i++) {
        dummyAdaptor.pushNewData();
    }

    int waitRounds = 0;
    if (dummyAdaptor.getDataCount() != dbusEmitter.numSamplesReceived() && waitRounds++ < MAX_WAIT_ROUNDS) {
        QTest::qWait(WAIT_ROUND_DELAY);
    }

    filterBin.stop();
    marshallingBin.stop();

    QCOMPARE (dummyAdaptor.getDataCount(), dbusEmitter.numSamplesReceived());

    delete coordAlignFilter;
}

// TODO: Add some state changes to verify functionality of threshold setting.
void FilterApiTest::testTopEdgeInterpretationFilter()
{
    // Input data to feed to the filter
    TimedXyzData inputData[] = {
        TimedXyzData(0,-981,   0,   0),
        TimedXyzData(0, 981,   0,   0),
        TimedXyzData(0,   0,-981,   0),
        TimedXyzData(0,   0, 981,   0)
    };

    // Expected output data
    PoseData expectedResult[] = {
        PoseData(PoseData::RightUp),
        PoseData(PoseData::LeftUp),
        PoseData(PoseData::BottomDown),
        PoseData(PoseData::BottomUp),
    };

    QVERIFY2((sizeof(inputData)/sizeof(TimedXyzData))==(sizeof(expectedResult)/sizeof(PoseData)),
             "Test function error: Input and output count does not match.");

    int numInputs = (sizeof(inputData)/sizeof(TimedXyzData));

    // Build data pipeline
    Bin filterBin;
    DummyAdaptor<TimedXyzData> dummyAdaptor;

    FilterBase* topEdgeInterpreterFilter = OrientationInterpreter::factoryMethod();

    RingBuffer<PoseData> outputBuffer(10);

    filterBin.add(&dummyAdaptor, "adapter");
    filterBin.add(topEdgeInterpreterFilter, "filter");
    filterBin.add(&outputBuffer, "buffer");
    filterBin.join("adapter", "source", "filter", "accsink");
    filterBin.join("filter", "topedge", "buffer", "sink");

    DummyDbusEmitter<PoseData> dbusEmitter;
    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdbusemitter");
    outputBuffer.join(&dbusEmitter);

    // Setup data
    dummyAdaptor.setTestData(numInputs, inputData, false);
    dbusEmitter.setExpectedData(numInputs, expectedResult, false);

    marshallingBin.start();
    filterBin.start();

    // Start sends data once, so start from index 1.
    for (int i=1; i < numInputs; i++) {
        dummyAdaptor.pushNewData();
    }

    int waitRounds = 0;
    if (dummyAdaptor.getDataCount() != dbusEmitter.numSamplesReceived() && waitRounds++ < MAX_WAIT_ROUNDS) {
        QTest::qWait(WAIT_ROUND_DELAY);
    }

    filterBin.stop();
    marshallingBin.stop();

    QCOMPARE (dummyAdaptor.getDataCount(), dbusEmitter.numSamplesReceived());

    delete topEdgeInterpreterFilter;
}

void FilterApiTest::testFaceInterpretationFilter()
{

    // Input data to feed to the filter
    TimedXyzData inputData[] = {
        TimedXyzData(0,   0,   0, -981),
        TimedXyzData(0,   0,   0,  981),
        TimedXyzData(0,   0,   0, -981),
        TimedXyzData(0,   0,   0, -981),
        TimedXyzData(0,   0,   0,  981),
        TimedXyzData(0,   0,   0,  981),
        TimedXyzData(0,   0,   0, -981),
        TimedXyzData(0,   0,   0, -981),
        TimedXyzData(0,   0,   0, -981)
    };

    // Expected output data for starting from FaceDown
    PoseData expectedResultDown[] = {   
        PoseData(PoseData::FaceUp),
        PoseData(PoseData::FaceDown),
        PoseData(PoseData::FaceUp)
    };


    // Expected output data for starting from FaceUp
    PoseData expectedResultUp[] = {
       
        PoseData(PoseData::FaceDown),
        PoseData(PoseData::FaceUp)
    };

    int numInputs = (sizeof(inputData)/sizeof(TimedXyzData));
    int numInputsDown = (sizeof(expectedResultDown)/sizeof(PoseData));
    int numInputsUp = (sizeof(expectedResultUp)/sizeof(PoseData));
    
    FilterBase* faceInterpreterFilter = OrientationInterpreter::factoryMethod();
    
    for (int i = 0; i < 2; i++)
    {
        Bin filterBin;
        DummyAdaptor<TimedXyzData> dummyAdaptor;

        if (i == 0){
            ((OrientationInterpreter*)faceInterpreterFilter)->orientation().orientation_ == PoseData::FaceDown;
        } else {
            ((OrientationInterpreter*)faceInterpreterFilter)->orientation().orientation_ == PoseData::FaceUp;
        }

        RingBuffer<PoseData> outputBuffer(10);

        filterBin.add(&dummyAdaptor, "adapter");
        filterBin.add(faceInterpreterFilter, "filter");
        filterBin.add(&outputBuffer, "buffer");
        filterBin.join("adapter", "source", "filter", "accsink");
        filterBin.join("filter", "face", "buffer", "sink");


        DummyDbusEmitter<PoseData> dbusEmitter;
        Bin marshallingBin;
        marshallingBin.add(&dbusEmitter, "testdbusemitter");
        outputBuffer.join(&dbusEmitter);

        // Setup data
        dummyAdaptor.setTestData(numInputs, inputData, false);
 
        if (i==0){
            dbusEmitter.setExpectedData(numInputsDown, expectedResultDown, false);   
        } else {
            dbusEmitter.setExpectedData(numInputsUp, expectedResultUp, false);  
        }

        marshallingBin.start();
        filterBin.start();

        for (int j=1; j < numInputs; j++){
            dummyAdaptor.pushNewData();
            QTest::qWait(300);
        }   

        int waitRounds = 0;

        if (dummyAdaptor.getDataCount() != (dbusEmitter.numSamplesReceived() + 6) && waitRounds++ < MAX_FACE_WAIT_ROUNDS) {
            QTest::qWait(WAIT_ROUND_DELAY);
        }

        filterBin.stop();
        marshallingBin.stop();

        if (i==0){  
            QCOMPARE (dummyAdaptor.getDataCount(), (dbusEmitter.numSamplesReceived() + 6));
        } else {         
            QCOMPARE (dummyAdaptor.getDataCount(), (dbusEmitter.numSamplesReceived() + 7));
        }
    }

    delete faceInterpreterFilter;
}


void FilterApiTest::testOrientationInterpretationFilter()
{
    // Input data to feed to the filter
    TimedXyzData inputData[] = {
        TimedXyzData(0,-981,   0,   0),
        TimedXyzData(0, 981,   0,   0),
        TimedXyzData(0,   0,-981,   0),
        TimedXyzData(0,   0, 981,   0),
        TimedXyzData(0,   0,   0, -981),
        TimedXyzData(0,   0,   0,  981)
    };

    // Expected output data
    PoseData expectedResult[] = {
        PoseData(PoseData::RightUp),
        PoseData(PoseData::LeftUp),
        PoseData(PoseData::BottomDown),
        PoseData(PoseData::BottomUp),
        PoseData(PoseData::FaceUp),
        PoseData(PoseData::FaceDown)
    };

    QVERIFY2((sizeof(inputData)/sizeof(TimedXyzData))==(sizeof(expectedResult)/sizeof(PoseData)),
             "Test function error: Input and output count does not match.");

    int numInputs = (sizeof(inputData)/sizeof(TimedXyzData));

    // Build data pipeline
    Bin filterBin;
    DummyAdaptor<TimedXyzData> dummyAdaptor;

    FilterBase* orientationInterpreterFilter = OrientationInterpreter::factoryMethod();

    RingBuffer<PoseData> outputBuffer(10);

    filterBin.add(&dummyAdaptor, "adapter");
    filterBin.add(orientationInterpreterFilter, "orientationfilter");
    filterBin.add(&outputBuffer, "buffer");
    filterBin.join("adapter", "source", "orientationfilter", "accsink");
    filterBin.join("orientationfilter", "orientation", "buffer", "sink");

    DummyDbusEmitter<PoseData> dbusEmitter;
    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdbusemitter");
    outputBuffer.join(&dbusEmitter);

    // Setup data
    dummyAdaptor.setTestData(numInputs, inputData, false);
    dbusEmitter.setExpectedData(numInputs, expectedResult, false);

    marshallingBin.start();
    filterBin.start();

    // Start sends data once, so start from index 1.
    for (int i=1; i < numInputs; i++) {
        dummyAdaptor.pushNewData();
    }

    int waitRounds = 0;
    if (dummyAdaptor.getDataCount() != dbusEmitter.numSamplesReceived() && waitRounds++ < MAX_WAIT_ROUNDS) {
        QTest::qWait(WAIT_ROUND_DELAY);
    }

    filterBin.stop();
    marshallingBin.stop();

    QCOMPARE (dummyAdaptor.getDataCount(), dbusEmitter.numSamplesReceived());

    delete orientationInterpreterFilter;
}

void FilterApiTest::testDeclinationFilter()
{
    // Input data to feed to the filter
    CompassData inputData[] = {
        CompassData(0, 0, 0),
        CompassData(0, 1, 0),
        CompassData(0, 0, 2),
        CompassData(0, 0, 0),
        CompassData(0, 4, 5),
        CompassData(0, 6, 0),
        CompassData(0, 0, 8),
        CompassData(0, 1, 1)
    };

    FilterBase* declinationFilter = DeclinationFilter::factoryMethod();
    int key = qvariant_cast<int>(dynamic_cast<DeclinationFilter*>(declinationFilter)->property("declinationCorrection"));

    // Expected output data
    CompassData expectedResult[] = {
        CompassData(0, 0+key, 0),
        CompassData(0, 1+key, 0),
        CompassData(0, 0+key, 2),
        CompassData(0, 0+key, 0),
        CompassData(0, 4+key, 5),
        CompassData(0, 6+key, 0),
        CompassData(0, 0+key, 8),
        CompassData(0, 1+key, 1)
    };

    QVERIFY2((sizeof(inputData))==(sizeof(expectedResult)),
             "Test function error: Input and output count does not match.");

    int numInputs = (sizeof(inputData)/sizeof(TimedXyzData));

    Bin filterBin;
    DummyAdaptor<CompassData> dummyAdaptor;

    RingBuffer<CompassData> outputBuffer(10);

    filterBin.add(&dummyAdaptor, "adapter");
    filterBin.add(declinationFilter, "declinationfilter");
    filterBin.add(&outputBuffer, "buffer");

    filterBin.join("adapter", "source", "declinationfilter", "sink");
    filterBin.join("declinationfilter", "source", "buffer", "sink");

    DummyDbusEmitter<CompassData> dbusEmitter;
    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdbusemitter");
    outputBuffer.join(&dbusEmitter);

    /* Setup data */
    dummyAdaptor.setTestData(numInputs, inputData, false);
    dbusEmitter.setExpectedData(numInputs, expectedResult, false);

    marshallingBin.start();
    filterBin.start();

    // Start sends data once, so start from index 1.
    for (int i=1; i < numInputs; i++) {
        dummyAdaptor.pushNewData();
    }

    int waitRounds = 0;
    if (dummyAdaptor.getDataCount() != dbusEmitter.numSamplesReceived() && waitRounds++ < MAX_WAIT_ROUNDS) {
        QTest::qWait(WAIT_ROUND_DELAY);
    }

    filterBin.stop();
    marshallingBin.stop();

    QCOMPARE (dummyAdaptor.getDataCount(), dbusEmitter.numSamplesReceived());

    delete declinationFilter;
}

void FilterApiTest::testRotationFilter()
{
    TimedXyzData inputData[] = {
        TimedXyzData(0,   0,   0,   0),
        TimedXyzData(1,   0,   0,-500),
        TimedXyzData(2,   0,   0, 500),

        TimedXyzData(3,   0,-500,   0),
        TimedXyzData(4,   0, 500,   0),

        TimedXyzData(5,-500,   0,   0),
        TimedXyzData(6, 500,   0,   0),

        TimedXyzData(7,-500,   0,-500),
        //~ TimedXyzData(8, 500,   0,-500),

        TimedXyzData(9,   0,-500,-500),
        //~ TimedXyzData(10,   0, 500,-500),

        TimedXyzData(11,   0,-500, 500),
        //~ TimedXyzData(12,   0, 500, 500),

    };

    TimedXyzData expectedResult[] = {
        TimedXyzData(0,   0,   0,   0),
        TimedXyzData(1,   0,   0,   0),
        TimedXyzData(2,   0, 180,   0),

        TimedXyzData(3,  90,   0,   0),
        TimedXyzData(4, -90,   0,   0),

        TimedXyzData(5,   0, -90,   0),
        TimedXyzData(6,   0,  90,   0),

        TimedXyzData(7,   0, -45,   0),
        //~ TimedXyzData(8,   0,  45,   0),

        TimedXyzData(9,  45,   0,   0),
        //~ TimedXyzData(10, -45,   0,   0),

        TimedXyzData(11,  45, 180,   0),
        //~ TimedXyzData(12, -45, 180,   0),

    };

    QVERIFY2((sizeof(inputData))==(sizeof(expectedResult)),
             "Test function error: Input and output count does not match.");

    int numInputs = (sizeof(inputData)/sizeof(TimedXyzData));

    DummyAdaptor<TimedXyzData> dummyAdaptor;
    DummyDbusEmitter<TimedXyzData> dbusEmitter;

    FilterBase* rotationFilter = RotationFilter::factoryMethod();
    RingBuffer<TimedXyzData> outputBuffer(10);

    Bin filterBin;
    filterBin.add(&dummyAdaptor, "adapter");
    filterBin.add(rotationFilter, "rotationfilter");
    filterBin.add(&outputBuffer, "buffer");

    filterBin.join("adapter", "source", "rotationfilter", "accelerometersink");
    filterBin.join("rotationfilter", "source", "buffer", "sink");

    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdbusemitter");
    outputBuffer.join(&dbusEmitter);

    /* Setup data */
    dummyAdaptor.setTestData(numInputs, inputData, false);
    dbusEmitter.setExpectedData(numInputs, expectedResult, false);

    marshallingBin.start();
    filterBin.start();

    // Start sends data once, so start from index 1.
    for (int i=1; i < numInputs; i++) {
        dummyAdaptor.pushNewData();
    }

    int waitRounds = 0;
    if (dummyAdaptor.getDataCount() != dbusEmitter.numSamplesReceived() && waitRounds++ < MAX_WAIT_ROUNDS) {
        QTest::qWait(WAIT_ROUND_DELAY);
    }

    filterBin.stop();
    marshallingBin.stop();

    QCOMPARE (dummyAdaptor.getDataCount(), dbusEmitter.numSamplesReceived());

    delete rotationFilter;
}

QTEST_MAIN(FilterApiTest)
