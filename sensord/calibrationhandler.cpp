/**
   @file calibrationhandler.cpp
   @brief Helper class for maintaining magnetometer calibration

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>

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

#define BG_RATE 1000
#define CALIBRATION_RATE 100
#define STOPPED_RATE 1000
#define CALIB_TIMEOUT 900000


CalibrationHandler::CalibrationHandler(QObject* parent) : QObject(parent),
    m_sensor(NULL),
    m_sessionId(-1),
    m_sensorName("magnetometersensor"),
    m_level(-1),
    m_active(false)
{
    m_timer = new QTimer(this);
}

CalibrationHandler::~CalibrationHandler()
{
    if (m_sessionId > 0)
    {
        SensorManager& sm = SensorManager::instance();
        sm.releaseSensor(m_sensorName, m_sessionId);
        m_sensor = NULL;
    }

    delete m_timer;
}

bool CalibrationHandler::initiateSession()
{

    SensorManager& sm = SensorManager::instance();
    sensordLogD() << "Loading MagnetometerSensorPlugin";
    if (!sm.loadPlugin(m_sensorName))
    {
        sensordLogW() << "Failed to load magnetometer plug-in";
        return false;
    }

    m_sessionId = sm.requestListenSensor(m_sensorName);
    if (m_sessionId <= 0)
    {
        sensordLogW() << "Failed to get listen session for magnetometersensor.";
    } else
    {
        m_sensor = reinterpret_cast<MagnetometerSensorChannel*>(sm.getSensorInstance(m_sensorName).sensor_);
        m_sensor->start();
        //~ sm.propertyHandler().setRequest("interval", "magnetometeradaptor", m_sessionId, BG_RATE);
        m_sensor->setIntervalRequest(m_sessionId, BG_RATE);
    }

    /// Connect data
    connect(m_sensor, SIGNAL(internalData(const MagneticField&)), this, SLOT(sampleReceived(const MagneticField&)));

    /// Connect timeout
    connect(m_timer, SIGNAL(timeout()), this, SLOT(stopCalibration()));

    m_active = true;

    return true;
}

void CalibrationHandler::sampleReceived(const MagneticField& sample)
{
    // Resume background calibration if magnetometer becomes active
    // due to some external reason (i.e. we receive values while inactive)

 if (m_active == false)
    {
        SensorManager& sm = SensorManager::instance();
        if (!sm.getPSMState())
        {
            resumeCalibration();
        }
    }

    if ((sample.level() != m_level))
    {
        m_level = sample.level();


        if (sample.level() < 3)
        {
            // Raise frequency if calibration is not at level 3
            m_sensor->setIntervalRequest(m_sessionId, CALIBRATION_RATE);


            // Set timer to stop calibration if it does not happen.
            // We only end up here if level has changed --> reset timer each time.
            m_timer->start(CALIB_TIMEOUT);

        } else
        {
            // Drop frequency when valid calibration has been reached.
            m_sensor->setIntervalRequest(m_sessionId, BG_RATE);

            m_timer->stop();
        }
    }
}

void CalibrationHandler::stopCalibration()
{
    if (!m_active)
    {
        return;
    }

    sensordLogD() << "Stopping magnetometer background calibration due to timeout.";
    // TODO: Release the sensor?
    m_sensor->setIntervalRequest(m_sessionId, STOPPED_RATE);

    m_timer->stop();

    // Sleep for a moment to let the queue get empty (kludgy...)
    QTimer::singleShot(1000, this, SLOT(setActiveToFalse()));
    //~ m_active = false;
}

void CalibrationHandler::setActiveToFalse()
{
    m_active = false;
}

void CalibrationHandler::resumeCalibration()
{
    if (m_active)
    {
        return;
    }
    sensordLogD() << "Resuming magnetometer background calibration.";
    m_sensor->setIntervalRequest(m_sessionId, BG_RATE);

    m_active = true;

    // Set internal level to -1 to ascertain that the next received sample
    // defines the resumed rate.
    m_level = -1;

}
