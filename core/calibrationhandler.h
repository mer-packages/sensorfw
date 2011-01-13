/**
   @file calibrationhandler.h
   @brief Helper class for maintaining magnetometer calibration

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
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord. If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#ifndef CALIBRATION_HANDLER
#define CALIBRATION_HANDLER

#include <QObject>
#include <QString>
#include <QTimer>
#include "datatypes/magneticfield.h"
#include "sensors/magnetometersensor/magnetometersensor.h"

/**
 * @brief Helper class for maintaining magnetometer calibration.
 *
 * Keeps a session open to magnetometer with low frequency to maintain
 * calibration.
 */
class CalibrationHandler : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CalibrationHandler)

public:
    /**
     * Constructor.
     * @param parent Parent QObject.
     */
    CalibrationHandler(QObject* parent = 0);
    ~CalibrationHandler();

    /**
     * @return \c true on success, \c false on failure.
     */
    bool initiateSession();

public slots:
    void sampleReceived(const MagneticField&);
    void stopCalibration();
    void resumeCalibration();
    void calibrationTimeout();

private:
    MagnetometerSensorChannel* m_sensor;
    int                        m_sessionId;
    QString                    m_sensorName;
    int                        m_level;
    QTimer                     m_timer;
    int                        calibration_rate;
    int                        calib_timeout;
};

#endif // CALIBRATION_HANDLER
