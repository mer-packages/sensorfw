/**
   @file tapadaptor.h
   @brief TapAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#ifndef TAPADAPTOR_H
#define TAPADAPTOR_H

#include "inputdevadaptor.h"
#include "sensord/deviceadaptorringbuffer.h"
#include "filters/tapdata.h"

/**
 * @brief Adaptor class for detecting device tap events.
 *
 * Tap events are recognized by the internal accelerometer driver, and passed
 * to sensor framework by this adaptor.
 *
 * @todo Add support for additional tap directions (only X, Y, Z now)
 * @todo Add support for adjusting sensor sensitivity
 * @todo Add detection of correct event handle (currently hardcoded to 4)
 */
class TapAdaptor : public InputDevAdaptor
{
    Q_OBJECT;
public:
    /**
     * Factory method for gaining a new instance of TapAdaptor class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new TapAdaptor(id);
    }

protected:
    /**
     * Constructor.
     * @param id Identifier for the adaptor.
     */
    TapAdaptor(const QString& id);
    ~TapAdaptor();

private:
    TapData tapValue_;
    DeviceAdaptorRingBuffer<TapData>* tapBuffer_; /**< Output buffer */

    bool waitingForDouble;
    int timerId;

    void interpretEvent(int src, struct input_event *ev);
    void interpretSync(int src);

    void timerEvent(QTimerEvent* event);
    void commitOutput();
};

#endif
