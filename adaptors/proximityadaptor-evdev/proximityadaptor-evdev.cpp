/**
   @file proximityadaptor-evdev.cpp
   @brief ProximityAdaptor utilizing evdev kernel interface

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Markus Lehtonen <markus.lehtonen@nokia.com>

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

#include <QtDebug>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>
#include <sensord/logging.h>
#include "filters/utils.h"
#include "config.h"

#include "proximityadaptor-evdev.h"

#define SELFDEF_EV_SW 5
#define SELFDEF_EV_FRONT_PROXIMITY 11

ProximityAdaptorEvdev::ProximityAdaptorEvdev(const QString& id) :
    InputDevAdaptor(id, 1), currentState_(ProximityStateUnknown)
{

    //This was previously in the base class, but it's not
    //possible call virtual methods from base class constructor.
    //TODO: find a way to get rid of all child classes calling this
    //manually.
    if (!getInputDevices(Config::configuration()->value("proximity-evdev_match_name").toString())) {
        sensordLogW() << "Input device not found.";
    }

    proximityBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(3);
    addAdaptedSensor("proximity", "Proximity state", proximityBuffer_);

    introduceAvailableDataRange(DataRange(-1, 1, 1));
}

ProximityAdaptorEvdev::~ProximityAdaptorEvdev()
{
    delete proximityBuffer_;
}

void ProximityAdaptorEvdev::interpretEvent(int src, struct input_event *ev)
{
    Q_UNUSED(src);

    switch (ev->type) {
        case SELFDEF_EV_SW:
        {
            switch (ev->code) {
                case SELFDEF_EV_FRONT_PROXIMITY:
                {
                    if (ev->value == 0) {
                        currentState_ = ProximityStateOpen;
                    }
                    else if (ev->value == 1) {
                        currentState_ = ProximityStateClosed;
                    }
                    else {
                        currentState_ = ProximityStateUnknown;
                    }
                }
                break;
                default: break;
            }
        }
        break;
        default: break;
    }
}

void ProximityAdaptorEvdev::interpretSync(int src)
{
    Q_UNUSED(src);
    commitOutput();
}

void ProximityAdaptorEvdev::commitOutput()
{

    sensordLogD() << "Proximity state change detected: " << currentState_;

    TimedUnsigned *proximityData = proximityBuffer_->nextSlot();

    proximityData->timestamp_ = Utils::getTimeStamp();
    proximityData->value_ = currentState_;

    proximityBuffer_->commit();
    proximityBuffer_->wakeUpReaders();
}

int ProximityAdaptorEvdev::getPollingInterval()
{
    return 0;
}

bool ProximityAdaptorEvdev::setPollingInterval(int f)
{
    Q_UNUSED(f);
    return false;
}
