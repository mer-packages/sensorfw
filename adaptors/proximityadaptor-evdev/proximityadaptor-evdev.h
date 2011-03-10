/**
   @file proximityadaptor-evdev.h
   @brief ProximityAdaptor utilizing evdev kernel interface

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Markus Lehtonen <markus.lehtonen@nokia.com>
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

#ifndef PROXIMITYADAPTOR_EVDEV_H
#define PROXIMITYADAPTOR_EVDEV_H

#include "inputdevadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/orientationdata.h"

/**
 * @brief Adaptor for proximity sensor.
 *
 * Events are received through @e evdev kernel interface.
 *
 * Listening clients are notified whenever the state has changed.
 */
class ProximityAdaptorEvdev : public InputDevAdaptor
{
    Q_OBJECT;
public:
    /* Enumerates the possible states for proximity */
    enum ProximityState {
        ProximityStateUnknown = -1, /**< Proximity state unknown */
        ProximityStateOpen = 0,     /**< Proximity open (no target detected)*/
        ProximityStateClosed = 1    /**< Proximity closed (target nearby)*/
    };

    /**
     * Factory method for gaining a new instance of ProximityAdaptorEvdev class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new ProximityAdaptorEvdev(id);
    }

protected:
    /**
     * Constructor.
     * @param id Identifier for the adaptor.
     */
    ProximityAdaptorEvdev(const QString& id);
    ~ProximityAdaptorEvdev();

private:

    DeviceAdaptorRingBuffer<ProximityData>*   proximityBuffer_;
    ProximityState                            currentState_;

    void interpretEvent(int src, struct input_event *ev);
    void commitOutput(struct input_event *ev);
    void interpretSync(int src, struct input_event *ev);
};

#endif
