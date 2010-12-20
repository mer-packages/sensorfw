/**
   @file stabilitybin.h
   @brief Stability Bin for ContextFW
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>
   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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

#include "stabilitybin.h"
#include "contextplugin.h"
#include "sensord/sensormanager.h"
#include "sensord/config.h"
#include "sensord/logging.h"

const int StabilityBin::STABILITY_THRESHOLD = 7;
const int StabilityBin::UNSTABILITY_THRESHOLD = 300;
const float StabilityBin::STABILITY_HYSTERESIS = 0.1;

StabilityBin::StabilityBin(ContextProvider::Service& s):
    isStableProperty(s, "Position.Stable"),
    isShakyProperty(s, "Position.Shaky"),
    accelerometerReader(10),
    cutterFilter(4.0),
    avgVarFilter(100),
    stabilityFilter(&isStableProperty, &isShakyProperty, STABILITY_THRESHOLD, UNSTABILITY_THRESHOLD, STABILITY_HYSTERESIS),
    sessionId(0)
{
    accelerometerAdaptor = SensorManager::instance().requestDeviceAdaptor("accelerometeradaptor");
    if (!accelerometerAdaptor)
    {
        sensordLogC() << "Unable to access Accelerometer for stability properties.";
    }

    add(&accelerometerReader, "accelerometer");
    add(&normalizerFilter, "normalizerfilter");
    add(&cutterFilter, "cutterfilter");
    add(&avgVarFilter, "avgvarfilter");
    add(&stabilityFilter, "stabilityfilter");

    join("accelerometer", "source", "normalizerfilter", "sink");
    join("normalizerfilter", "source", "cutterfilter", "sink");
    join("cutterfilter", "source", "avgvarfilter", "sink");
    join("avgvarfilter", "source", "stabilityfilter", "sink");

    RingBufferBase* rb = accelerometerAdaptor->findBuffer("accelerometer");
    if (!rb)
    {
        sensordLogC() << "Unable to connect to accelerometer.";
    } else {
        rb->join(&accelerometerReader);
    }
    // Context group
    group.add(isStableProperty);
    group.add(isShakyProperty);
    connect(&group, SIGNAL(firstSubscriberAppeared()), this, SLOT(startRun()));
    connect(&group, SIGNAL(lastSubscriberDisappeared()), this, SLOT(stopRun()));
}

StabilityBin::~StabilityBin()
{
    RingBufferBase* rb = accelerometerAdaptor->findBuffer("accelerometer");
    if (rb)
    {
        rb->unjoin(&accelerometerReader);
    }
    SensorManager::instance().releaseDeviceAdaptor("accelerometeradaptor");
}

void StabilityBin::startRun()
{
    // Get unique sessionId for this Bin.
    sessionId = SensorManager::instance().requestListenSensor("contextsensor");
    if (sessionId == INVALID_SESSION)
    {
        sensordLogC() << "Failed to get unique id for stability detection.";
    }

    // Reset the status of the avg & var computation; reset default
    // values for properties whose values aren't reliable after a
    // restart
    avgVarFilter.reset();
    isStableProperty.unsetValue();
    isShakyProperty.unsetValue();
    start();
    accelerometerAdaptor->startSensor("accelerometer");

    accelerometerAdaptor->setStandbyOverrideRequest(sessionId, true);
}

void StabilityBin::stopRun()
{
    accelerometerAdaptor->setStandbyOverrideRequest(sessionId, false);
    accelerometerAdaptor->stopSensor("accelerometer");
    stop();
    SensorManager::instance().releaseSensor("contextsensor", sessionId);
}
