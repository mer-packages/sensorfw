/**
   @file filtertests.cpp
   @brief Automatic tests for filters
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>

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

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"
#include "filter.h"
#include "coordinatealignfilter.h"
#include "orientationdata.h"
#include "orientationinterpreter.h"
#include "declinationfilter.h"
#include "rotationfilter.h"
#include "filtertests.h"
#include "config.h"
#include <QSettings>

void FilterApiTest::initTestCase()
{
    Config::loadConfig(CONFIG_FILE_PATH, CONFIG_DIR_PATH);
}

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

    QVERIFY2((sizeof(inputData)) == (sizeof(expectedResult)),
             "Test function error: Input and output count does not match.");

    int numInputs = (sizeof(inputData) / sizeof(TimedXyzData));

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

    DummyDataEmitter<TimedXyzData> dbusEmitter;
    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdataemitter");
    outputBuffer.join(&dbusEmitter);

    /* Setup data */
    dummyAdaptor.setTestData(numInputs, inputData);
    dbusEmitter.setExpectedData(numInputs, expectedResult);

    marshallingBin.start();
    filterBin.start();

    // Start sends data once, so start from index 1.
    for (int i = 0; i < numInputs; ++i) {
        dummyAdaptor.pushNewData();
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
        TimedXyzData(       0,-981,   0,   0),
        TimedXyzData(10000000, 981,   0,   0),
        TimedXyzData(20000000,   0,-981,   0),
        TimedXyzData(30000000,   0, 981,   0)
    };

    // Expected output data
    PoseData expectedResult[] = {
        PoseData(       0, PoseData::RightUp),
        PoseData(10000000, PoseData::LeftUp),
        PoseData(20000000, PoseData::BottomDown),
        PoseData(30000000, PoseData::BottomUp),
    };

    QVERIFY2((sizeof(inputData) / sizeof(TimedXyzData)) ==
             (sizeof(expectedResult) / sizeof(PoseData)),
             "Test function error: Input and output count does not match.");

    int numInputs = (sizeof(inputData) / sizeof(TimedXyzData));

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

    DummyDataEmitter<PoseData> dbusEmitter;
    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdataemitter");
    outputBuffer.join(&dbusEmitter);

    // Setup data
    dummyAdaptor.setTestData(numInputs, inputData);
    dbusEmitter.setExpectedData(numInputs, expectedResult);

    marshallingBin.start();
    filterBin.start();

    // Start sends data once, so start from index 1.
    for (int i = 0; i < numInputs; ++i) {
        dummyAdaptor.pushNewData();
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
        TimedXyzData(      0,   0,   0,-981),
        TimedXyzData(1000000,   0,   0, 981),
        TimedXyzData(2000000,   0,   0,-981),
        TimedXyzData(2100000,   0,   0, 981),
        TimedXyzData(2200000,   0,   0,-981)
    };

    // Expected output data
    PoseData expectedResult[] = {
        PoseData(      0, PoseData::FaceUp),
        PoseData(1000000, PoseData::FaceDown),
        PoseData(2000000, PoseData::FaceUp),
    };

    int numInputs = (sizeof(inputData) / sizeof(TimedXyzData));
    int numOutputs = (sizeof(expectedResult) / sizeof(PoseData));

    FilterBase* faceInterpreterFilter = OrientationInterpreter::factoryMethod();

    Bin filterBin;
    DummyAdaptor<TimedXyzData> dummyAdaptor;

    RingBuffer<PoseData> outputBuffer(10);

    filterBin.add(&dummyAdaptor, "adapter");
    filterBin.add(faceInterpreterFilter, "filter");
    filterBin.add(&outputBuffer, "buffer");
    filterBin.join("adapter", "source", "filter", "accsink");
    filterBin.join("filter", "face", "buffer", "sink");

    DummyDataEmitter<PoseData> dbusEmitter;
    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdataemitter");
    outputBuffer.join(&dbusEmitter);

    // Setup data
    dummyAdaptor.setTestData(numInputs, inputData);
    dbusEmitter.setExpectedData(numOutputs, expectedResult);

    marshallingBin.start();
    filterBin.start();

    for (int j=1; j < numInputs; j++){
        dummyAdaptor.pushNewData();
    }

    filterBin.stop();
    marshallingBin.stop();

    QVERIFY2(numOutputs == dbusEmitter.numSamplesReceived(), "Too many/few outputs from filter.");

    delete faceInterpreterFilter;
}

void FilterApiTest::testOrientationInterpretationFilter()
{
    // Input data to feed to the filter
    TimedXyzData inputData[] = {
        TimedXyzData(      0,-981,   0,   0),
        TimedXyzData(1000000, 981,   0,   0),
        TimedXyzData(2000000,   0,-981,   0),
        TimedXyzData(3000000,   0, 981,   0),
        TimedXyzData(4000000,   0,   0, -981),
        TimedXyzData(5000000,   0,   0,  981)
    };

    // Expected output data
    PoseData expectedResult[] = {
        PoseData(      0, PoseData::RightUp),
        PoseData(1000000, PoseData::LeftUp),
        PoseData(2000000, PoseData::BottomDown),
        PoseData(3000000, PoseData::BottomUp),
        PoseData(4000000, PoseData::FaceUp),
        PoseData(5000000, PoseData::FaceDown)
    };

    QVERIFY2((sizeof(inputData) / sizeof(TimedXyzData)) ==
             (sizeof(expectedResult) / sizeof(PoseData)),
             "Test function error: Input and output count does not match.");

    int numInputs = (sizeof(inputData) / sizeof(TimedXyzData));

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

    DummyDataEmitter<PoseData> dbusEmitter;
    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdataemitter");
    outputBuffer.join(&dbusEmitter);

    // Setup data
    dummyAdaptor.setTestData(numInputs, inputData);
    dbusEmitter.setExpectedData(numInputs, expectedResult);

    marshallingBin.start();
    filterBin.start();

    // Start sends data once, so start from index 1.
    for (int i = 0; i < numInputs; ++i) {
        dummyAdaptor.pushNewData();
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
        CompassData(0, 340, 5),
        CompassData(0, 359, 0),
        CompassData(0, 0, 8),
        CompassData(0, 1, 1)
    };

    FilterBase* declinationFilter = DeclinationFilter::factoryMethod();
    QVERIFY(declinationFilter);

    QSettings confFile("/etc/xdg/sensorfw/location.conf", QSettings::IniFormat);
    confFile.beginGroup("location");
    confFile.setValue("declination",50);

    int key = dynamic_cast<DeclinationFilter*>(declinationFilter)->declinationCorrection();

    QCOMPARE(key, 50);

    // Expected output data
    CompassData expectedResult[] = {
        CompassData(0, 0, 0, 0 + key, 0),
        CompassData(0, 1, 0, 1 + key, 1),
        CompassData(0, 0, 2, 0 + key, 0),
        CompassData(0, 0, 0, 0 + key, 0),
        CompassData(0, 340, 5, 30, 340),
        CompassData(0, 359, 0, 49, 359),
        CompassData(0, 0, 8, 0 + key, 0),
        CompassData(0, 1, 1, 1 + key, 1)
    };

    QVERIFY2((sizeof(inputData)) == (sizeof(expectedResult)),
             "Test function error: Input and output count does not match.");

    int numInputs = (sizeof(inputData) / sizeof(TimedXyzData));

    Bin filterBin;
    DummyAdaptor<CompassData> dummyAdaptor;

    RingBuffer<CompassData> outputBuffer(10);

    filterBin.add(&dummyAdaptor, "adapter");
    filterBin.add(declinationFilter, "declinationfilter");
    filterBin.add(&outputBuffer, "buffer");

    filterBin.join("adapter", "source", "declinationfilter", "sink");
    filterBin.join("declinationfilter", "source", "buffer", "sink");

    DummyDataEmitter<CompassData> dbusEmitter;
    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdataemitter");
    outputBuffer.join(&dbusEmitter);

    /* Setup data */
    dummyAdaptor.setTestData(numInputs, inputData);
    dbusEmitter.setExpectedData(numInputs, expectedResult);

    marshallingBin.start();
    filterBin.start();

    // Start sends data once, so start from index 1.
    for (int i = 0; i < numInputs; ++i) {
        dummyAdaptor.pushNewData();
    }

    filterBin.stop();
    marshallingBin.stop();

    QCOMPARE (dummyAdaptor.getDataCount(), dbusEmitter.numSamplesReceived());

    delete declinationFilter;

    confFile.setValue("declination",0);
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

    QVERIFY2((sizeof(inputData)) == (sizeof(expectedResult)),
             "Test function error: Input and output count does not match.");

    int numInputs = (sizeof(inputData) / sizeof(TimedXyzData));

    DummyAdaptor<TimedXyzData> dummyAdaptor;
    DummyDataEmitter<TimedXyzData> dbusEmitter;

    FilterBase* rotationFilter = RotationFilter::factoryMethod();
    RingBuffer<TimedXyzData> outputBuffer(10);

    Bin filterBin;
    filterBin.add(&dummyAdaptor, "adapter");
    filterBin.add(rotationFilter, "rotationfilter");
    filterBin.add(&outputBuffer, "buffer");

    filterBin.join("adapter", "source", "rotationfilter", "accelerometersink");
    filterBin.join("rotationfilter", "source", "buffer", "sink");

    Bin marshallingBin;
    marshallingBin.add(&dbusEmitter, "testdataemitter");
    outputBuffer.join(&dbusEmitter);

    /* Setup data */
    dummyAdaptor.setTestData(numInputs, inputData);
    dbusEmitter.setExpectedData(numInputs, expectedResult);

    marshallingBin.start();
    filterBin.start();

    // Start sends data once, so start from index 1.
    for (int i = 0; i < numInputs; ++i) {
        dummyAdaptor.pushNewData();
    }

    filterBin.stop();
    marshallingBin.stop();

    QCOMPARE (dummyAdaptor.getDataCount(), dbusEmitter.numSamplesReceived());

    delete rotationFilter;
}

QTEST_MAIN(FilterApiTest)
