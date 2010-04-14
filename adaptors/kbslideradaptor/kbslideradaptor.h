/**
   @file kbslideradaptor.h
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

#ifndef KBSLIDERADAPTOR_H
#define KBSLIDERADAPTOR_H

#include "inputdevadaptor.h"
#include "sensord/deviceadaptorringbuffer.h"

/**
 * @brief Adaptor for device keyboard slider state.
 *
 * Events are received through @e gpio-keys input layer.
 *
 * Listening clients are notified whenever the state has changed.
 *
 * @todo Move definition of \e KeyboardSliderState to filters/kbsliderstate.
 */
class KeyboardSliderAdaptor : public InputDevAdaptor
{
    Q_OBJECT;
public:
    /* Enumerates the possible states for keyboard */
    enum KeyboardSliderState {
        KeyboardSliderStateOpen = 0, /**< Keyboard slider open */
        KeyboardSliderStateClosed,   /**< Keyboard slider closed */
        KeyboardSliderStateUnknown   /**< Keyboard slider state unknown */
    };

    /**
     * Factory method for gaining a new instance of KeyboardSliderAdaptor class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new KeyboardSliderAdaptor(id);
    }

protected:
    /**
     * Constructor.
     * @param id Identifier for the adaptor.
     */
    KeyboardSliderAdaptor(const QString& id);
    ~KeyboardSliderAdaptor();

    virtual int getPollingInterval();
    virtual bool setPollingInterval(int f);

private:

    DeviceAdaptorRingBuffer<KeyboardSliderState>* kbstateBuffer_;
    bool                                          newKbEventRecorded_;
    KeyboardSliderState                           currentState_;

    void interpretEvent(int src, struct input_event *ev);
    void commitOutput();
    void interpretSync(int src);
};

#endif

