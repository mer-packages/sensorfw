/**
   @file tapadaptor.h
   @brief TapAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>

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
#include "deviceadaptorringbuffer.h"
#include "datatypes/tapdata.h"

/**
 * @brief Adaptor class for detecting device tap events.
 *
 * Tap events are recognized by the internal accelerometer driver, and passed
 * to sensor framework by this adaptor.
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

    virtual bool setInterval(const unsigned int value, const int sessionId);

private:
    DeviceAdaptorRingBuffer<TapData>* tapBuffer_; /**< Output buffer */

    void interpretEvent(int src, struct input_event *ev);
    void interpretSync(int src, struct input_event *ev);
    void commitOutput(const TapData& data);
};

#endif
