/**
   @file pusher.h
   @brief Pusher

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>

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

#ifndef PUSHER_H
#define PUSHER_H

#include "producer.h"
#include "callback.h"

/**
 * Base-class for pusher type of data producers.
 */
class Pusher : public Producer
{
public:
    /**
     * Constructor.
     */
    Pusher();

    /**
     * Set callback to be called when new data is pushed.
     *
     * @param ready Callback.
     */
    void setReadyCallback(const CallbackBase* ready);

    /**
     * Invoke callback.
     */
    void wakeup() const;

    /**
     * Push new data.
     */
    virtual void pushNewData() = 0;

protected:
    const CallbackBase* ready_; /**< callback */
};

#endif
