/**
   @file compassbin.cpp
   @brief Compass Bin for ContextFW
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>
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

#include "compassbin.h"
#include "sensord/sensormanager.h"

#include <QDebug>

// TODO: ContextSensor always has session ID 2, but this should be taken from SM,
// not written here.
#define SESSION_ID 2

CompassBin::CompassBin(ContextProvider::Service& s, bool pluginValid):
    headingProperty(s, "Location.Heading"),
    compassChain(0),
    compassReader(10),
    headingFilter(&headingProperty)
{
    if (pluginValid)
    {
        compassChain = SensorManager::instance().requestChain("compasschain");
        Q_ASSERT(compassChain);

        add(&compassReader, "compass");
        add(&headingFilter, "headingfilter");

        // Create a branching filter chain
        join("compass", "source", "headingfilter", "sink");

        RingBufferBase* rb;
        rb = compassChain->findBuffer("truenorth");
        Q_ASSERT(rb);
        rb->join(&compassReader);
    }

    // Listening for context property subscriptions
    connect(&headingProperty, SIGNAL(firstSubscriberAppeared(QString)), this, SLOT(startRun()));
    connect(&headingProperty, SIGNAL(lastSubscriberDisappeared(QString)), this, SLOT(stopRun()));
}

CompassBin::~CompassBin()
{
}

void CompassBin::startRun()
{
    start();
    if (compassChain) {
        compassChain->start();
        compassChain->setProperty("compassEnabled", true);

        // Set interval for compass, as sane operation requires 10HZ!
        SensorManager::instance().propertyHandler().setRequest("interval", "accelerometeradaptor", SESSION_ID, 100);
        SensorManager::instance().propertyHandler().setRequest("interval", "magnetometeradaptor", SESSION_ID, 100);
    }
}

void CompassBin::stopRun()
{
    if (compassChain) {
        // Release interval requests...
        SensorManager::instance().propertyHandler().setRequest("interval", "accelerometeradaptor", SESSION_ID, 0);
        SensorManager::instance().propertyHandler().setRequest("interval", "magnetometeradaptor", SESSION_ID, 0);

        compassChain->stop();
        compassChain->setProperty("compassEnabled", false);
    }
    stop();
}
