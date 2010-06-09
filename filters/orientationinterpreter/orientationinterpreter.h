/**
   @file orientationinterpreter.h
   @brief OrientationInterpreter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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
#include <sensord/filter.h>
#include <sensord/filters/orientationdata.h>
#include <sensord/filters/posedata.h>
#include "sensord/filterproperty.h"

/**
 * @brief Filter for calculating device orientation.
 *
 * Filter for calculating the device orientation. Input from the
 * #TopEdgeFilter, #FaceFilter and #AccelerometerAdaptor is used.
 *
 */
class OrientationInterpreter : public QObject, public PropertyTracker, public FilterBase
{
    Q_OBJECT;

    Q_PROPERTY(PoseData orientation READ orientation);
private:
    Sink<OrientationInterpreter, PoseData> faceSink;
    Sink<OrientationInterpreter, PoseData> topEdgeSink;
    Sink<OrientationInterpreter, AccelerationData> accDataSink;
    Source<PoseData> orientationSource;

    void faceDataAvailable(unsigned, const PoseData*);
    void topEdgeDataAvailable(unsigned, const PoseData*);
    void accDataAvailable(unsigned, const AccelerationData*);

    OrientationInterpreter();

    PoseData topEdge;
    PoseData face;

    PoseData o_;

public:
    /**
     * Factory method for CompassFilter.
     * @return New CompassFilter as FilterBase*.
     */
    static FilterBase* factoryMethod()
    {
        return new OrientationInterpreter();
    }

    PoseData orientation() const {
        return o_;
    }
};

#endif // ORIENTATIONINTERPRETER_H
