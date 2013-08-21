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

CompassFilter::CompassFilter() :
        magDataSink(this, &CompassFilter::magDataAvailable),
        accelSink(this, &CompassFilter::accelDataAvailable),
        orientDataSink(this, &CompassFilter::orientDataAvailable),
        factor(1)
{
    addSink(&magDataSink, "magsink");
    addSink(&accelSink, "accsink");
    addSink(&orientDataSink, "orientsink");

    addSource(&magSource, "magnorthangle");
}

void CompassFilter::magDataAvailable(unsigned, const CalibratedMagneticFieldData *data)
{
    adjX = data->x_;
    adjY = data->y_;
    adjZ = data->z_;

    magRX = data->rx_;
    magRY = data->ry_;
    magRZ = data->rz_;

    level = data->level_;
}

void CompassFilter::accelDataAvailable(unsigned, const AccelerationData *data)
{
    qreal x = data->x_ * 0.00980665; // to m/s^2
    qreal y = data->y_ * 0.00980665;
    qreal z = data->z_ * 0.00980665;
    ///////////////
    /// \brief this algorithm is from Circuit Cellar Aug 2012
    ///  by Mark Pedley
    /// Electronic Compass: Tilt Compensation & Calibration
    /// There are no restrictions on your use of the software listed in the
    /// Circuit Cellar magazine.
    /// http://circuitcellar.com/
    ///
    qreal Psi = 0;
    qreal The = 0;
    qreal Phi = 0; /* yaw, pitch, roll angles in deg */

    qreal Bx = magRX;
    qreal By = magRY;
    qreal Bz = magRZ;
    qreal Gx = x;
    qreal Gy = y;
    qreal Gz = z;

    qreal sinAngle, cosAngle; /* sine and cosine */
    qreal fBfx;
    qreal fBfy;
    qreal fBfz; /* calibrated mag data in uT after tilt correction */

    /* subtract off the hard iron interference computed using equation 9*/

    Bx -= adjX;
    By -= adjY;
    Bz -= adjZ;

    /* calculate roll angle Phi (-180deg, 180deg) and sin, cos */

    Phi = qAtan2(Gy, Gz) * RADIANS_TO_DEGREES; /* Equation 2 */
    sinAngle = qSin(Phi * DEGREES_TO_RADIANS);  /* sin(Phi) */
    cosAngle = qCos(Phi * DEGREES_TO_RADIANS);  /* cos(Phi) */

    /* de-rotate by roll angle Phi */
    fBfy = By * cosAngle - Bz * sinAngle; /* Equation 5 y component */

    Bz = By * sinAngle + Bz * cosAngle; /*Bz=(By-Vy).sin(Phi)+(Bz-Vz).cos(Phi) */
    Gz = Gy * sinAngle + Gz * cosAngle; /* Gz=Gy.sin(Phi)+Gz.cos(Phi) */

    /* calculate pitch angle Theta (-90deg, 90deg) and sin, cos*/
    The = atan(-Gx / Gz) * RADIANS_TO_DEGREES;  /* Equation 3 */
    sinAngle = qSin(The * DEGREES_TO_RADIANS); /* sin(Theta) */
    cosAngle = qCos(The * DEGREES_TO_RADIANS); /* cos(Theta) */

    /* de-rotate by pitch angle Theta */
    fBfx = Bx * cosAngle + Bz * sinAngle; /* Equation 5 x component */
    fBfz = -Bx * sinAngle + Bz * cosAngle; /* Equation 5 z component */

    /* calculate yaw = ecompass angle psi (-180deg, 180deg) */

    Psi = (qAtan2(-fBfy, fBfx) * RADIANS_TO_DEGREES); /* Equation 7 */


// because sensorfw expects x,y axis to be opposite from what this algo expects
int offset = 90;
    ////////////////////////////////////////
    CompassData compassData; //north angle
    compassData.timestamp_ = data->timestamp_;
    compassData.degrees_ = (int)(Psi + (360 - offset)) % 360;
    compassData.level_ = level;
    magSource.propagate(1, &compassData);
}

void CompassFilter::orientDataAvailable(unsigned, const TimedXyzData *data)
{
    CompassData compassData; //north angle
    compassData.timestamp_ = data->timestamp_;
    compassData.degrees_ = data->x_;
    compassData.level_ = level;
    qDebug() << Q_FUNC_INFO << compassData.degrees_ * .001;

//    magSource.propagate(1, &compassData);
}
