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
#include "magnetometersensor_i.h"
#include "datatypes/magneticfield.h"
#include "datatypes/xyz.h"

class ClientApiTest : public QObject
{
    Q_OBJECT;

public:

    ClientApiTest();

private:
    void calcAverages(const QVector<XYZ>& data, float& x, float& y,  float& z) const;
    void calcMaggeAverages(const QVector<MagneticField>& data, float& x, float& y,  float& z, float& rx, float& ry, float& rz) const;
    long getLimit(AbstractSensorChannelInterface* sensor) const;
    static AbstractSensorChannelInterface* getSensor(const QString& sensorName);
    QStringList bufferingSensors;
    template<typename T>
    QByteArray errorMessage(const QString& sensorName, int interval, const T& value, const QString& compOperator, const T& limit) const;

private slots:
    // Setup tests
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    // Sensors
    void testAccelerometerSensor();
    void testGyroscopeSensor();
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
    virtual void dataAvailable(const MagneticField&);
    virtual void frameAvailable(const QVector<MagneticField>& frame);
    virtual void dataAvailable2(const XYZ&);
    virtual void frameAvailable2(const QVector<XYZ>& frame);

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

    QVector<MagneticField> getSamples1() { return samples1; }
    QVector<XYZ> getSamples2() { return samples2; }

public Q_SLOTS:
    virtual void dataAvailable(const MagneticField&);
    virtual void frameAvailable(const QVector<MagneticField>& frame);
    virtual void dataAvailable2(const XYZ&);
    virtual void frameAvailable2(const QVector<XYZ>& frame);

private:
    QVector<MagneticField> samples1;
    QVector<XYZ> samples2;
};

template<typename T>
QByteArray ClientApiTest::errorMessage(const QString& sensorName, int interval, const T& value, const QString& compOperator, const T& limit) const
{
    QString errMsg(QString("Buffering err %1 (%2 Hz): ").arg(sensorName).arg(QString::number(1000 / interval)));
    errMsg.append(QString("%1 %2 %3").arg(QString::number(value)).arg(compOperator).arg(QString::number(limit)));
    return errMsg.toLatin1();
}

#endif // CLIENT_API_TEST_H
