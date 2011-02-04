/**
   @file orientationbin.cpp
   @brief Orientation Bin for ContextFW
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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
#include "sensormanager.h"
#include "config.h"

const int OrientationBin::POLL_INTERVAL = 250;

OrientationBin::OrientationBin(ContextProvider::Service& s):
    topEdgeProperty(s, "Screen.TopEdge"),
    isCoveredProperty(s, "Screen.IsCovered"),
    isFlatProperty(s, "Position.IsFlat"),
    accelerometerReader(10),
    topEdgeReader(10),
    faceReader(10),
    screenInterpreterFilter(&topEdgeProperty, &isCoveredProperty, &isFlatProperty),
    sessionId(0)
{
    add(&topEdgeReader, "topedge");
    add(&faceReader, "face");
    add(&screenInterpreterFilter, "screeninterpreterfilter");

    // Create a branching filter chain
    join("topedge", "source", "screeninterpreterfilter", "sink");
    join("face", "source", "screeninterpreterfilter", "sink");

    // Context group
    group.add(topEdgeProperty);
    group.add(isCoveredProperty);
    group.add(isFlatProperty);
    connect(&group, SIGNAL(firstSubscriberAppeared()), this, SLOT(startRun()));
    connect(&group, SIGNAL(lastSubscriberDisappeared()), this, SLOT(stopRun()));

    // Set default values (if the default isn't Unknown)
    topEdgeProperty.setValue("top");
    isCoveredProperty.setValue(false);
    isFlatProperty.setValue(false);
}

OrientationBin::~OrientationBin()
{
    stopRun();
}

void OrientationBin::startRun()
{
    // Get unique sessionId for this Bin.
    sessionId = SensorManager::instance().requestSensor("contextsensor");
    if (sessionId == INVALID_SESSION)
    {
        sensordLogC() << "Failed to get unique id for orientation detection.";
    }

    orientationChain = SensorManager::instance().requestChain("orientationchain");
    if (!orientationChain)
    {
        sensordLogC() << "Unable to access OrientationChain for orientation properties.";
        return;
    }

    RingBufferBase* rb = orientationChain->findBuffer("topedge");
    if (!rb)
    {
        sensordLogC() << "Unable to connect to TopEdge orientation buffer.";
    } else {
        rb->join(&topEdgeReader);
    }

    rb = orientationChain->findBuffer("face");
    if (!rb)
    {
        sensordLogC() << "Unable to connect to face orientation buffer.";
    } else {
        rb->join(&faceReader);
    }

    start();
    orientationChain->start();

    unsigned int pollInterval = Config::configuration()->value("orientation_poll_interval", QVariant(POLL_INTERVAL)).toUInt();
    orientationChain->setIntervalRequest(sessionId, pollInterval);
}

void OrientationBin::stopRun()
{
    stop();
    if (orientationChain)
    {
        orientationChain->requestDefaultInterval(sessionId);

        orientationChain->stop();

        RingBufferBase* rb = orientationChain->findBuffer("topedge");
        if (rb)
        {
            rb->unjoin(&topEdgeReader);
        }

        rb = orientationChain->findBuffer("face");
        if (rb)
        {
            rb->unjoin(&faceReader);
        }

        SensorManager::instance().releaseChain("orientationchain");
        orientationChain = NULL;
    }

    SensorManager::instance().releaseSensor("contextsensor", sessionId);
}
