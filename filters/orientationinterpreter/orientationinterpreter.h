/**
   @file orientationinterpreter.h
   @brief OrientationInterpreter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>
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
#ifndef ORIENTATIONINTERPRETER_H
#define ORIENTATIONINTERPRETER_H

#include <QObject>
#include <QAtomicInt>
#include "filter.h"
#include <datatypes/orientationdata.h>
#include <datatypes/posedata.h>

/**
 * @brief Filter for calculating device orientation.
 *
 * Filter for calculating the device orientation. Input from
 * #AccelerometerChain is used.
 *
 */

class OrientationInterpreter : public QObject, public FilterBase
{
    Q_OBJECT;

    Q_PROPERTY(PoseData orientation READ orientation);
    Q_PROPERTY(int threshold READ threshold WRITE setThreshold);

private:
    Sink<OrientationInterpreter, AccelerationData> accDataSink;
    Source<PoseData> topEdgeSource;
    Source<PoseData> faceSource;
    Source<PoseData> orientationSource;

    void accDataAvailable(unsigned, const AccelerationData*);

    bool overFlowCheck();
    void processTopEdge();
    void processFace();
    void processOrientation();

    OrientationInterpreter();

    QAtomicInt threshold_;

    PoseData topEdge;
    PoseData face;
    PoseData previousFace;
    bool updatePreviousFace;

    AccelerationData data;
    QList<AccelerationData> dataBuffer;

    int minlimit;
    int maxlimit;
    int angleThresholdPortrait;
    int angleThresholdLandscape;
    unsigned long discardTime;

    PoseData o_;
    enum OrientationMode
    {
        Portrait = 0, /**< Orientation mode is portrait. */
        Landscape     /**< Orientation mode is landscape */
    };

    PoseData rotateToLandscape(int);
    PoseData rotateToPortrait(int);
    int orientationCheck(const AccelerationData&, OrientationMode) const;
    PoseData orientationRotation(const AccelerationData&, OrientationMode, PoseData (OrientationInterpreter::*)(int));

    static const int DEFAULT_THRESHOLD;
    static const float RADIANS_TO_DEGREES;
    static const int ANGLE_LIMIT;
    static const int SAME_AXIS_LIMIT;

    static const int OVERFLOW_MIN;
    static const int OVERFLOW_MAX;

    static const int THRESHOLD_LANDSCAPE;
    static const int THRESHOLD_PORTRAIT;

    static const int DISCARD_TIME;
    static const int AVG_BUFFER_MAX_SIZE;

public:
    /**
     * Factory method.
     * @return New OrientationInterpreter as FilterBase*.
     */
    static FilterBase* factoryMethod()
    {
        return new OrientationInterpreter();
    }

    PoseData orientation() const {
        return o_;
    }

    int threshold() const { return threshold_; }

    void setThreshold(int threshold) { threshold_ = threshold; }
};

#endif
