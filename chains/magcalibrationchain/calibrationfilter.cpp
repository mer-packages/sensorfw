/****************************************************************************
**
** Copyright (C) 2013-2015 Jolla Ltd
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

#include "calibrationfilter.h"
#include "config.h"
#include "sensormanager.h"
#include <QFile>
#include <QTextStream>
/*
 * I've left in routines to grab calibrated and uncalibrated data
 * in order to use data plotting to visualize calibrations.
 *
 * */
#define DATA_POINTS 5000
//#define CALIBRATE_DATA

CalibrationFilter::CalibrationFilter() :
    Filter<CalibratedMagneticFieldData, CalibrationFilter, CalibratedMagneticFieldData>(this, &CalibrationFilter::magDataAvailable),
    magDataSink(this, &CalibrationFilter::magDataAvailable),
    calLevel(0),
    bufferPos(0),
    dataPoints(0)
{
    addSink(&magDataSink, "magsink");
    addSource(&magSource, "calibratedmagneticfield");
// min, max
    minMaxList.insert(0,qMakePair(0,0));
    minMaxList.insert(1,qMakePair(0,0));
    minMaxList.insert(2,qMakePair(0,0));

    manualCalibration = Config::configuration()->value<bool>("magnetometer/needs_calibration", false);

    qDebug() << Q_FUNC_INFO << manualCalibration;
#ifdef CALIBRATE_DATA
    unCalibratedData.setFileName("sensor.csv");
    calibratedData.setFileName("sensor-calibrated.csv");

    if (!unCalibratedData.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        qDebug() << "could not open data file";
    else {
        stream.setDevice(&unCalibratedData);
        stream << "X,Y,Z" << endl;
    }
    if (!calibratedData.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        qDebug() << "could not open calibratedData file";
    else {
        calibratedStream.setDevice(&calibratedData);
        calibratedStream << "X,Y,Z" << endl;
    }
#endif
}

void CalibrationFilter::magDataAvailable(unsigned, const CalibratedMagneticFieldData *data)
{
    transformed.timestamp_ = data->timestamp_;
    transformed.x_ = data->rx_;
    transformed.y_ = data->ry_;
    transformed.z_ = data->rz_;
    transformed.level_ = data->level_;

    if (manualCalibration) {

        //    simple hard iron correction
        if (minMaxList.at(0).first == 0) {
            minMaxList.replace(0,qMakePair(data->rx_, data->rx_));
            minMaxList.replace(1,qMakePair(data->ry_, data->ry_));
            minMaxList.replace(2,qMakePair(data->rz_, data->rz_));

        } else {
            minMaxList.replace(0,qMakePair(qMin(minMaxList.at(0).first, data->rx_),
                                           qMax(minMaxList.at(0).second, data->rx_)));
            minMaxList.replace(1,qMakePair(qMin(minMaxList.at(1).first, data->ry_),
                                           qMax(minMaxList.at(1).second, data->ry_)));
            minMaxList.replace(2,qMakePair(qMin(minMaxList.at(2).first, data->rz_),
                                           qMax(minMaxList.at(2).second, data->rz_)));
        }
        meanX = ((minMaxList.at(0).first + minMaxList.at(0).second) * .5);
        meanY = ((minMaxList.at(1).first + minMaxList.at(1).second) * .5);
        meanZ = ((minMaxList.at(2).first + minMaxList.at(2).second) * .5);

        if (calLevel != 3
                || meanX != offsetX || meanY != offsetY || meanZ != offsetZ) {

            calLevel = 0;
            if (offsetX == meanX)
                calLevel += 1;
            if (offsetY == meanY)
                calLevel += 1;
            if (offsetZ == meanZ)
                calLevel += 1;

            offsetX = meanX;
            offsetY = meanY;
            offsetZ = meanZ;

            transformed.level_ = calLevel;

            transformed.x_ -= offsetX;
            transformed.y_ -= offsetY;
            transformed.z_ -= offsetZ;

            ///////////////////// soft iron
            qreal vmaxX = minMaxList.at(0).second - ((minMaxList.at(0).first + minMaxList.at(0).second) * 0.5);
            qreal vmaxY = minMaxList.at(1).second - ((minMaxList.at(1).first + minMaxList.at(1).second) * 0.5);
            qreal vmaxZ = minMaxList.at(2).second - ((minMaxList.at(2).first + minMaxList.at(2).second) * 0.5);

            qreal vminX = minMaxList.at(0).first - ((minMaxList.at(0).first + minMaxList.at(0).second) * 0.5);
            qreal vminY = minMaxList.at(1).first - ((minMaxList.at(1).first + minMaxList.at(1).second) * 0.5);
            qreal vminZ = minMaxList.at(2).first - ((minMaxList.at(2).first + minMaxList.at(2).second) * 0.5);

            qreal avgX = vmaxX + (vminX * -1);
            avgX = avgX * 0.5;
            qreal avgY = vmaxY + (vminY * -1);
            avgY = avgY * 0.5;
            qreal avgZ = vmaxZ + (vminZ * -1);
            avgZ = avgZ * 0.5;

            qreal avgRad = avgX + avgY + avgZ;
            avgRad /= 3.0;

            xScale = (avgRad/avgX);
            yScale = (avgRad/avgY);
            zScale = (avgRad/avgZ);
        }

        transformed.x_ *= xScale;
        transformed.y_ *= yScale;
        transformed.z_ *= zScale;

    }
#ifdef CALIBRATE_DATA
    if (dataPoints == DATA_POINTS) {
        unCalibratedData.close();
        calibratedData.close();
    } else {
        stream << data->x_ * 0.001 << "," << data->y_  * 0.001 << "," << data->z_ * 0.001  << endl;
        calibratedStream << transformed.x_  * 0.001 << "," << transformed.y_  * 0.001 << "," << transformed.z_  * 0.001 << endl;
        dataPoints++;
    }
#endif
    transformed.rx_ = data->rx_;
    transformed.ry_ = data->ry_;
    transformed.rz_ = data->rz_;

    magSource.propagate(1, &transformed);
    source_.propagate(1, &transformed);
}

void CalibrationFilter::dropCalibration()
{
    calLevel = 0;
    minMaxList.clear();
}
