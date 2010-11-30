/**
   @file orientationbin.cpp
   @brief Orientation Bin for ContextFW
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>
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

#include "orientationbin.h"
#include "contextplugin.h"
#include "sensord/sensormanager.h"
#include "sensord/config.h"

const int OrientationBin::STABILITY_THRESHOLD = 7;
const int OrientationBin::UNSTABILITY_THRESHOLD = 300;
const float OrientationBin::STABILITY_HYSTERESIS = 0.1;
const int OrientationBin::POLL_INTERVAL = 250;

OrientationBin::OrientationBin(ContextProvider::Service& s):
    topEdgeProperty(s, "Screen.TopEdge"),
    isCoveredProperty(s, "Screen.IsCovered"),
    isFlatProperty(s, "Position.IsFlat"),
    isStableProperty(s, "Position.Stable"),
    isShakyProperty(s, "Position.Shaky"),
    accelerometerReader(10),
    topEdgeReader(10),
    faceReader(10),
    screenInterpreterFilter(&topEdgeProperty, &isCoveredProperty, &isFlatProperty),
    cutterFilter(4.0),
    avgVarFilter(100),
    stabilityFilter(&isStableProperty, &isShakyProperty,
                    STABILITY_THRESHOLD, UNSTABILITY_THRESHOLD, STABILITY_HYSTERESIS)
{
    accelerometerAdaptor = SensorManager::instance().requestDeviceAdaptor("accelerometeradaptor");
    Q_ASSERT(accelerometerAdaptor);

    orientationChain = SensorManager::instance().requestChain("orientationchain");
    Q_ASSERT(orientationChain);

    add(&accelerometerReader, "accelerometer");
    add(&topEdgeReader, "topedge");
    add(&faceReader, "face");
    add(&screenInterpreterFilter, "screeninterpreterfilter");
    add(&normalizerFilter, "normalizerfilter");
    add(&cutterFilter, "cutterfilter");
    add(&avgVarFilter, "avgvarfilter");
    add(&stabilityFilter, "stabilityfilter");

    // Create a branching filter chain
    join("topedge", "source", "screeninterpreterfilter", "sink");
    join("face", "source", "screeninterpreterfilter", "sink");

    join("accelerometer", "source", "normalizerfilter", "sink");
    join("normalizerfilter", "source", "cutterfilter", "sink");
    join("cutterfilter", "source", "avgvarfilter", "sink");
    join("avgvarfilter", "source", "stabilityfilter", "sink");

    RingBufferBase* rb = accelerometerAdaptor->findBuffer("accelerometer");
    Q_ASSERT(rb);
    rb->join(&accelerometerReader);

    rb = orientationChain->findBuffer("topedge");
    Q_ASSERT(rb);
    rb->join(&topEdgeReader);

    rb = orientationChain->findBuffer("face");
    Q_ASSERT(rb);
    rb->join(&faceReader);

    // Context group
    group.add(topEdgeProperty);
    group.add(isCoveredProperty);
    group.add(isFlatProperty);
    group.add(isStableProperty);
    group.add(isShakyProperty);
    connect(&group, SIGNAL(firstSubscriberAppeared()), this, SLOT(startRun()));
    connect(&group, SIGNAL(lastSubscriberDisappeared()), this, SLOT(stopRun()));

    // Set default values (if the default isn't Unknown)
    topEdgeProperty.setValue("top");
    isCoveredProperty.setValue(false);
    isFlatProperty.setValue(false);
}

OrientationBin::~OrientationBin()
{
    RingBufferBase* rb = accelerometerAdaptor->findBuffer("accelerometer");
    Q_ASSERT(rb);
    rb->unjoin(&accelerometerReader);

    SensorManager::instance().releaseDeviceAdaptor("accelerometeradaptor");
}

void OrientationBin::startRun()
{
    // Reset the status of the avg & var computation; reset default
    // values for properties whose values aren't reliable after a
    // restart
    avgVarFilter.reset();
    isStableProperty.unsetValue();
    isShakyProperty.unsetValue();
    start();
    accelerometerAdaptor->startSensor("accelerometer");
    orientationChain->start();

    unsigned int pollInterval = Config::configuration()->value("orientation_poll_interval", QVariant(POLL_INTERVAL)).toUInt();
    orientationChain->setIntervalRequest(ContextPlugin::getSessionId(), pollInterval);
}

void OrientationBin::stopRun()
{
    orientationChain->requestDefaultInterval(ContextPlugin::getSessionId());

    accelerometerAdaptor->stopSensor("accelerometer");
    orientationChain->stop();
    stop();
}
