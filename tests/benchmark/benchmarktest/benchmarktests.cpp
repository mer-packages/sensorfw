/**
   @file benchmarktests.cpp
   @brief Automatic benchmarktests

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

#include "qt-api/sensormanagerinterface.h"
#include "qt-api/alssensor_i.h"

#include "benchmarktests.h"
#include "signaldump.h"

void BenchmarkTest::initTestCase()
{
    SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
    QVERIFY( remoteSensorManager.isValid() );

    // Load plugins (should test running depend on plug-in load result?)
    remoteSensorManager.loadPlugin("alssensor");

    // Register interfaces (can this be done inside the plugins?
    remoteSensorManager.registerSensorInterface<ALSSensorChannelInterface>("alssensor");
}

void BenchmarkTest::init()
{
}

void BenchmarkTest::cleanup()
{
}

void BenchmarkTest::cleanupTestCase()
{
}

void BenchmarkTest::testIdleMemCpu()
{
    // CPU activity won't be zero, as context stuff is likely to be
    // running.

    // Memory use limits could be set and verified
}

void BenchmarkTest::testThroughput()
{
    SignalDump signalDump;
    int DELAY = 10; // in seconds
    int MEMSPLITS = 4; // this + 1 = number of mem measurements

    // get sensord PID
    QProcess* process = new QProcess(this);
    process->start(QString("pidof sensord"));
    process->waitForReadyRead(1000);
    int sensordPid = atoi(process->readLine());
    process->close();
    process->waitForFinished();
    delete process;

    qDebug() << "Detected sensord PID:" << sensordPid;

    QString sensorName("alssensor");
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    QVERIFY( sm.isValid() );

    ALSSensorChannelInterface* sensorIfc = const_cast<ALSSensorChannelInterface*>(ALSSensorChannelInterface::listenInterface(sensorName));
    QVERIFY2(sensorIfc && sensorIfc->isValid(), "Failed to get listen session");

    qDebug() << "Listening to sensor data for" << DELAY << "seconds";

    // get mem
    signalDump.recordMemUsage(sensordPid);

    QDBusReply<void> reply = sensorIfc->start();
    connect(sensorIfc, SIGNAL(ALSChanged(const Unsigned&)), &signalDump, SLOT(counterSlot(const Unsigned&)));

    signalDump.getCpuUsage(sensordPid);

    // Run with few breaks for measuring memory usage.
    for (int i = 1; i < MEMSPLITS; i++) {
        QTest::qWait((DELAY*1000)/MEMSPLITS);
        signalDump.recordMemUsage(sensordPid);
    }
    QTest::qWait((DELAY*1000)/MEMSPLITS);

    double cpuUsage = signalDump.getCpuUsage(sensordPid);

    disconnect(sensorIfc, SIGNAL(ALSChanged(const Unsigned&)), &signalDump, SLOT(counterSlot(const Unsigned&)));
    reply = sensorIfc->stop();

    // get mem
    signalDump.recordMemUsage(sensordPid);

    /// Sample rate
    double sampleRate = signalDump.cnt/(double)DELAY;
    qDebug() << "[Sample Rate]:" << sampleRate << "s/sec";

    /// Cpu usage
    qDebug() << "[CPU usage  ]:" << cpuUsage*100 << "%";

    /// Memory usage
    int cleanMin = -1;
    int cleanMax = 0;
    int cleanAvg = 0;
    foreach (int value, signalDump.memoryClean) {
        if (value > cleanMax) cleanMax = value;
        if ( value < cleanMin || cleanMin == -1) cleanMin = value;
        cleanAvg += value;
    }
    cleanAvg = cleanAvg / signalDump.memoryClean.size();

    int dirtyMin = -1;
    int dirtyMax = 0;
    int dirtyAvg = 0;
    foreach (int value, signalDump.memoryDirty) {
        if (value > dirtyMax) dirtyMax = value;
        if ( value < dirtyMin || dirtyMin == -1) dirtyMin = value;
        dirtyAvg += value;
    }
    dirtyAvg = dirtyAvg / signalDump.memoryDirty.size();

    qDebug() << "[MEM        ]: Average Min Max" ;
    qDebug() << "[      Clean]:" << cleanAvg << cleanMin << cleanMax;
    qDebug() << "[      Dirty]:" << dirtyAvg << dirtyMin << dirtyMax;

    delete sensorIfc;
}

void BenchmarkTest::testSessionLeaks()
{
    int ITERATIONS = 30;
    SignalDump signalDump;

    qDebug() << "Initiating and closing RotationSensor" << ITERATIONS << "times";

    QProcess* process = new QProcess(this);
    process->start(QString("pidof sensord"));
    process->waitForReadyRead(1000);
    int sensordPid = atoi(process->readLine());
    process->close();
    process->waitForFinished();
    delete process;

    for (int i = 0; i < ITERATIONS; i++) {
        process = new QProcess(this);
        process->start(QString("sensordummyclient"));
        process->waitForFinished();
        delete process;
        if (i > 0) {
            signalDump.recordMemUsage(sensordPid);
        }
    }
    QTest::qWait(500); // let things settle...
    int deltaClean = signalDump.memoryClean.last() - signalDump.memoryClean.first();
    int deltaDirty = signalDump.memoryDirty.last() - signalDump.memoryDirty.first();

    qDebug() << "[MEM leakage]:";
    qDebug() << "[      Clean]:" << deltaClean << "bytes in total";
    qDebug() << "[           ]:" << deltaClean*1.0/ITERATIONS << "bytes / session";
    qDebug() << "[      Dirty]:" << deltaDirty << "bytes in total";
    qDebug() << "[           ]:" << deltaDirty*1.0/ITERATIONS << "bytes / session";
}

void BenchmarkTest::testLostSessionLeaks()
{
    int ITERATIONS = 30;
    SignalDump signalDump;

    qDebug() << "Initiating and killing RotationSensor" << ITERATIONS << "times";

    QProcess* process = new QProcess(this);
    process->start(QString("pidof sensord"));
    process->waitForReadyRead(1000);
    int sensordPid = atoi(process->readLine());
    process->close();
    process->waitForFinished();
    delete process;

    for (int i = 0; i < ITERATIONS; i++) {
        process = new QProcess(this);
        process->start(QString("sensordummyclient"));
        QTest::qWait(250);
        process->close();
        process->waitForFinished();
        delete process;
        if (i > 0) {
            signalDump.recordMemUsage(sensordPid);
        }
    }
    QTest::qWait(500); // let things settle...
    int deltaClean = signalDump.memoryClean.last() - signalDump.memoryClean.first();
    int deltaDirty = signalDump.memoryDirty.last() - signalDump.memoryDirty.first();

    qDebug() << "[MEM leakage]:";
    qDebug() << "[      Clean]:" << deltaClean << "bytes in total";
    qDebug() << "[           ]:" << deltaClean*1.0/ITERATIONS << "bytes / session";
    qDebug() << "[      Dirty]:" << deltaDirty << "bytes in total";
    qDebug() << "[           ]:" << deltaDirty*1.0/ITERATIONS << "bytes / session";
}

QTEST_MAIN(BenchmarkTest)
