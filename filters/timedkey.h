/**
   @file timedkey.h
   @brief Datatype for key events

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#ifndef TIMED_KEY_H
#define TIMED_KEY_H

#include "genericdata.h"
#include "ariane/ariane.h"

/**
 * @brief Datatype for unsigned integer value with timestamp.
 */
class TimedKey : public TimedData {
public:
    TimedKey()
        : TimedData(0)
    {
        value_.key      = 0;
        value_.pressed  = false;
        value_.released = false;
    }
    TimedKey(int timestamp, ariane_event_key_t value)
        : TimedData(timestamp), value_(value)
    {
        value_.key      = value.key;
        value_.pressed  = value.pressed;
        value_.released = value.released;
    }

    ariane_event_key_t value_;
};
Q_DECLARE_METATYPE ( TimedKey )


#endif // TIMED_UNSIGNED_H
