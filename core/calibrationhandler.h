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

    /**
     * Destructor.
     */
    ~CalibrationHandler();

    /**
     * Initialize object and start background calibration.
     *
     * @return true on success.
     */
    bool initiateSession();

public slots:
    /**
     * Callback when new sample is received from magnetometer.
     */
    void sampleReceived(const MagneticField&);

    /**
     * Stop calibration.
     */
    void stopCalibration();

    /**
     * Resume calibration.
     */
    void resumeCalibration();

private slots:
    /**
     * Callback for calibration.
     */
    void calibrationTimeout();

private:
    static const QString       SENSOR_NAME;    /**< magnetometer sensor name */

    MagnetometerSensorChannel* m_sensor;       /**< magnetometer sensor channel */
    int                        m_sessionId;    /**< session ID */
    int                        m_level;        /**< calibration level */
    QTimer                     m_timer;        /**< calibration timer */
    int                        m_calibRate;    /**< calibration rate */
    int                        m_calibTimeout; /**< calibration timeout */
};

#endif // CALIBRATION_HANDLER
