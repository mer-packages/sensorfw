/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com

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
 */
#include <QDebug>
#include "compassfilter.h"
#include "config.h"

#include <QtCore/qmath.h>

#define RADIANS_TO_DEGREES 57.2957795
#define DEGREES_TO_RADIANS 0.017453292
#define GRAVITY_EARTH 9.80665f
#define FILTER_FACTOR 0.24f
#define LIST_COUNT 10

CompassFilter::CompassFilter() :
        magDataSink(this, &CompassFilter::magDataAvailable),
        accelSink(this, &CompassFilter::accelDataAvailable),
        level(0),
        oldHeading(0)
{
    addSink(&magDataSink, "magsink");
    addSink(&accelSink, "accsink");
    addSource(&magSource, "magnorthangle");
}

void CompassFilter::magDataAvailable(unsigned, const CalibratedMagneticFieldData *data)
{
    magX = data->y_ * .001f;
    magY = data->x_ * .001f;
    magZ = data->z_ * .001f;
    level = data->level_;

    magX = oldMagX + FILTER_FACTOR * (magX - oldMagX);
    magY = oldMagY + FILTER_FACTOR * (magY - oldMagY);
    magZ = oldMagZ + FILTER_FACTOR * (magZ - oldMagZ);
    oldMagX = magX;
    oldMagY = magY;
    oldMagZ = magZ;
}


void CompassFilter::accelDataAvailable(unsigned, const AccelerationData *data)
{
    // the x/y are switched as compass expects it in aero coordinates
    qreal Gx = data->y_ * .001f; //convert to g
    qreal Gy = data->x_ * .001f;
    qreal Gz = -data->z_ * .001f;

    qreal divisor = qSqrt(Gx * Gx + Gy * Gy + Gz * Gz);
    qreal normalizedGx = Gx / divisor;
    qreal normalizedGy = Gy / divisor;
    qreal normalizedGz = Gz / divisor;

    ///////////////
    /// this algorithm is from Circuit Cellar Aug 2012
    ///  by Mark Pedley
    /// Electronic Compass: Tilt Compensation & Calibration
    /// There are no restrictions on your use of the software listed in the
    /// Circuit Cellar magazine.
    /// http://circuitcellar.com/
    ///
    qreal Psi = 0;
    qreal The = 0;
    qreal Phi = 0;
    qreal sinAngle = 0;
    qreal cosAngle = 0;
    qreal fBfx = 0;
    qreal fBfy = 0;

    /* calculate roll angle Phi (-180deg, 180deg) and sin, cos */
    Phi = qAtan2(normalizedGy, normalizedGz); /* Equation 2 */
    sinAngle = qSin(Phi);
    cosAngle = qCos(Phi);

    /* de-rotate magY roll angle Phi */
    fBfy = magY * cosAngle - magZ * sinAngle; /* Equation 5 y component */
    magZ = magY * sinAngle + magZ * cosAngle;
    normalizedGz = normalizedGy * sinAngle + normalizedGz * cosAngle;

    /* calculate pitch angle Theta (-90deg, 90deg) and sin, cos*/
    The = qAtan(-normalizedGx / normalizedGz);  /* Equation 3 */
    sinAngle = qSin(The);
    cosAngle = qCos(The);

    /* de-rotate magY pitch angle Theta */
    fBfx = magX * cosAngle + magZ * sinAngle; /* Equation 5 x component */

    /* calculate yaw = ecompass angle psi (-180deg, 180deg) */
    Psi = (qAtan2(-fBfy, fBfx) * RADIANS_TO_DEGREES); /* Equation 7 */

    int heading = Psi * FILTER_FACTOR + oldHeading * (1.0 - FILTER_FACTOR);

    CompassData compassData; //north angle
    compassData.timestamp_ = data->timestamp_;
    compassData.degrees_ = (int)(heading + 360) % 360;
    compassData.level_ = level;
    magSource.propagate(1, &compassData);
    oldHeading = heading;
}
