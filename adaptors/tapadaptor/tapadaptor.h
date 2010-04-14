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

#include "sysfsadaptor.h"
#include "sensord/deviceadaptorringbuffer.h"
#include "filters/tapdata.h"
#include <QTime>
#include <linux/input.h>

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
class TapAdaptor : public SysfsAdaptor
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
    input_event                       ev_;
    DeviceAdaptorRingBuffer<TapData>* tapBuffer_; /**< Output buffer */

    /**
     * Runs whenever an event has been received. Contains logic for figuring
     * which event was fired, and passes the data on to filterchain.
     * @param pathId PathId for the file that had event. Always 0, as we monitor
     *               only single file and don't set any proper id.
     * @param fd     Open file descriptor with new data. See #SysfsAdaptor::processSample()
     */
    void processSample(int pathId, int fd);
};

#endif
