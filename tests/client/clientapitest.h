/**
   @file clientapitest.h
   @brief Automatic tests for sensor client interfaces

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#ifndef CLIENT_API_TEST_H
#define CLIENT_API_TEST_H

#include <QTest>
#include <QVector>
#include <QStringList>
#include "qt-api/magnetometersensor_i.h"
#include "datatypes/magneticfield.h"
#include "datatypes/xyz.h"

class ClientApiTest : public QObject
{
    Q_OBJECT;

public: ClientApiTest();
    static QByteArray errorMessage(QString sensorName, int interval, int limit, int condition, int value);

private:
    void calcAverages(QVector<QObject*> data, long& x, long& y,  long& z);
    void calcMaggeAverages(QVector<QObject*> data, long& x, long& y,  long& z, long& rx, long& ry,  long& rz);
    long getLimit(AbstractSensorChannelInterface* sensor);
    static QStringList m_bufferingSensors;


private slots:
    // Setup tests
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    // Sensors
    void testAccelerometerSensor();
    void testOrientationSensor();
    void testRotationSensor();
    void testTapSensor();
    void testMagnetometerSensor();
    void testCompassSensor();
    void testALSSensor();
    void testProximitySensor();

    // Special cases
    void testCommonAdaptorPipeline();
    void testSessionInitiation();

    // Buffering
    void testBuffering();
    void testBufferingAllIntervalRanges();
    void testBufferingCompatibility();
    void testBufferingInterval();
    void testAvailableBufferIntervals();
    void testAvailableBufferSizes();

    // Downsampling
    void testDownsampling();
    void testDownsamplingDisabled();


};

class TestClient : public QObject
{
    Q_OBJECT;

public:
    TestClient(AbstractSensorChannelInterface& iface, bool listenFrames);
    virtual ~TestClient();

    int getDataCount() const { return dataCount; }
    int getFrameCount() const { return frameCount; }
    int getFrameDataCount() const { return frameDataCount; }
    void resetTimers();
    void resetCounters();

public Q_SLOTS:
    virtual void dataAvailable(const MagneticField&) { QTime now = QTime::currentTime(); qDebug() << "dataAvailable() "<< ++dataCount<<" in "<< (dataCount>-1?m_exTimeData.msecsTo(now):0)<< " ms"; m_exTimeData = now;}
    virtual void frameAvailable(const QVector<MagneticField>& frame) { QTime now = QTime::currentTime(); qDebug() << "frameAvailable(): " << frame.size()<< " in "<<(frameCount>-1?m_exTimeFrame.msecsTo(now):0)<< " ms"; m_exTimeFrame = now;; ++frameCount; frameDataCount += frame.size(); }
    virtual void dataAvailable2(const XYZ&) { QTime now = QTime::currentTime(); qDebug() << "dataAvailable() "<< ++dataCount<<" in "<< (dataCount>-1?m_exTimeData.msecsTo(now):0)<< " ms"; m_exTimeData = now;}
    virtual void frameAvailable2(const QVector<XYZ>& frame) { QTime now = QTime::currentTime(); qDebug() << "frameAvailable(): " << frame.size()<< " in "<<(frameCount>-1?m_exTimeFrame.msecsTo(now):0)<< " ms"; m_exTimeFrame = now;; ++frameCount; frameDataCount += frame.size(); }

private:
    int dataCount;
    long dataInterval;
    int frameCount;
    int frameDataCount;
    long frameInterval;
    QTime m_exTimeData;
    QTime m_exTimeFrame;
};

class SampleCollector : public TestClient
{
    Q_OBJECT;

public:
    SampleCollector(AbstractSensorChannelInterface& iface, bool listenFrames);
    virtual ~SampleCollector();

    QVector<QObject*> getSamples() { return samples; }

public Q_SLOTS:
    virtual void dataAvailable(const MagneticField&);
    virtual void frameAvailable(const QVector<MagneticField>& frame);
    virtual void dataAvailable2(const XYZ&);
    virtual void frameAvailable2(const QVector<XYZ>& frame);

private:
    QVector<QObject*> samples;
};

#endif // CLIENT_API_TEST_H
