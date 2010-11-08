/**
   @file orientationinterpreter.h
   @brief OrientationInterpreter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>

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
#include <QTimer>
#include <sensord/filter.h>
#include <datatypes/orientationdata.h>
#include <datatypes/posedata.h>
#include "sensord/filterproperty.h"

/**
 * @brief Filter for calculating device orientation.
 *
 * Filter for calculating the device orientation. Input from
 * #AccelerometerChain is used.
 *
 */
class OrientationInterpreter : public QObject, public PropertyTracker, public FilterBase
{
    Q_OBJECT;

    Q_PROPERTY(PoseData orientation READ orientation);
    Q_PROPERTY(int threshold READ threshold_ WRITE threshold_);

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

    FilterProperty<int> threshold_;

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
};

#endif
