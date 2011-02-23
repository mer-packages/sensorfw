/**
   @file kbslideradaptor.cpp
   @brief KeyboardsliderAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#include <QtDebug>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>
#include "logging.h"

#include "kbslideradaptor.h"

#define SELFDEF_EV_KB 5
#define SELFDEF_EV_KBSLIDE 10

KeyboardSliderAdaptor::KeyboardSliderAdaptor(const QString& id) :
    InputDevAdaptor(id, 1), newKbEventRecorded_(false), currentState_(KeyboardSliderStateUnknown)
{
    kbstateBuffer_ = new DeviceAdaptorRingBuffer<KeyboardSliderState>(1);
    setAdaptedSensor("keyboardslider", "Device keyboard slider state", kbstateBuffer_);
    setDescription("Keyboard slider events (via input device)");
}

KeyboardSliderAdaptor::~KeyboardSliderAdaptor()
{
    delete kbstateBuffer_;
}

void KeyboardSliderAdaptor::interpretEvent(int src, struct input_event *ev)
{
    Q_UNUSED(src);

    switch (ev->type) {
        case SELFDEF_EV_KB:
        {
            switch (ev->code) {
                case SELFDEF_EV_KBSLIDE:
                {
                    newKbEventRecorded_ = true;
                    if (ev->value == 0) {
                        currentState_ = KeyboardSliderStateClosed;
                    }
                    else if (ev->value == 1) {
                        currentState_ = KeyboardSliderStateOpen;
                    }
                    else {
                        currentState_ = KeyboardSliderStateUnknown;
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

void KeyboardSliderAdaptor::interpretSync(int src, struct input_event *ev)
{
    Q_UNUSED(src);
    Q_UNUSED(ev);
    commitOutput();
}

void KeyboardSliderAdaptor::commitOutput()
{

    sensordLogD() << "KB Slider state change detected: " << currentState_;

    KeyboardSliderState *state = kbstateBuffer_->nextSlot();

    *state = currentState_;

    kbstateBuffer_->commit();
    kbstateBuffer_->wakeUpReaders();
}

unsigned int KeyboardSliderAdaptor::interval() const
{
    return 0;
}

bool KeyboardSliderAdaptor::setInterval(unsigned int value, int sessionId)
{
    Q_UNUSED(value);
    Q_UNUSED(sessionId);
    return true;
}
