/**
   @file calibrationhandler.cpp
   @brief Helper class for maintaining magnetometer calibration

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord. If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#include "calibrationhandler.h"
#include "sensormanager.h"
#include "logging.h"
#include "abstractsensor.h"
#include "config.h"

const QString CalibrationHandler::SENSOR_NAME("magnetometersensor");

CalibrationHandler::CalibrationHandler(QObject* parent) :
    QObject(parent),
    m_sensor(NULL),
    m_sessionId(-1),
    m_level(-1)
{
    m_timer.setSingleShot(true);

    m_calibRate = Config::configuration()->value<int>("m_calibRate", 100);
    m_calibTimeout = Config::configuration()->value<int>("calibration_timeout", 60000);
}

CalibrationHandler::~CalibrationHandler()
{
    if (m_sessionId > 0)
    {
        SensorManager& sm = SensorManager::instance();
        sm.releaseSensor(SENSOR_NAME, m_sessionId);
        m_sensor = NULL;
    }
}

bool CalibrationHandler::initiateSession()
{
    SensorManager& sm = SensorManager::instance();
    sensordLogD() << "Loading MagnetometerSensorPlugin";
    if (!sm.loadPlugin(SENSOR_NAME))
    {
        sensordLogW() << "Failed to load magnetometer plug-in";
        return false;
    }

    m_sessionId = sm.requestSensor(SENSOR_NAME);
    if (m_sessionId <= 0)
    {
        sensordLogW() << "Failed to get session for magnetometersensor.";
    }
    else
    {
        m_sensor = reinterpret_cast<MagnetometerSensorChannel*>(sm.getSensorInstance(SENSOR_NAME)->sensor_);
    }

    // Connect timeout
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(calibrationTimeout()));
    resumeCalibration();
    return true;
}

void CalibrationHandler::sampleReceived(const MagneticField& sample)
{
    //Reset timer when level changes
    if ((sample.level() != m_level))
    {
        m_level = sample.level();
        m_timer.start(m_calibTimeout);
    }
}

void CalibrationHandler::stopCalibration()
{
    if (m_sensor && m_timer.isActive())
    {
        m_timer.stop();
        m_sensor->setStandbyOverrideRequest(m_sessionId, false);
        m_sensor->stop();
        disconnect(m_sensor, SIGNAL(internalData(const MagneticField&)), this, SLOT(sampleReceived(const MagneticField&)));
        sensordLogD() << "Stopping magnetometer background calibration due to PSM on";
    }
}

void CalibrationHandler::calibrationTimeout()
{
    if (m_sensor)
    {
        sensordLogD() << "Stopping magnetometer background calibration due to timeout.";
        m_sensor->setStandbyOverrideRequest(m_sessionId, false);
        m_sensor->stop();
        disconnect(m_sensor, SIGNAL(internalData(const MagneticField&)), this, SLOT(sampleReceived(const MagneticField&)));
    }
}

void CalibrationHandler::resumeCalibration()
{
    sensordLogD() << "Resuming magnetometer background calibration";
    if (m_sensor && !m_timer.isActive())
    {
        m_sensor->start();
        m_sensor->setIntervalRequest(m_sessionId, m_calibRate);
        m_sensor->setStandbyOverrideRequest(m_sessionId, true);
        connect(m_sensor, SIGNAL(internalData(const MagneticField&)), this, SLOT(sampleReceived(const MagneticField&)));
    }
    m_timer.start(m_calibTimeout);
}
