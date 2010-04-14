/**
   @file touchdata.h
   @brief Datatype for touch events

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#ifndef TOUCHDATA_H
#define TOUCHDATA_H

#include "genericdata.h"

/**
 * @brief Datatype for device touchscreen events.
 */
class TouchData : public TimedXyzData {
public:

    /**
     * Enumeration to handle synaptic multitouch driver accuracy. On certain 
     * occasions the measured coordinates might be reversed on x/y axis.
     */
    enum FingerState {
        FingerStateNotPresent = 0, /**< Finger not present */
        FingerStateAccurate,       /**< Coordinates are accurate */
        FingerStateInaccurate      /**< Coordinates are either accurate or mirrored. */
    };

    int tp_;         /**< Touch event source (which finger), 1,2,... */
    FingerState st_; /**< Touch event finger state */

    /** 
     * Constructor. Initialises all values as zero.
     */
    TouchData() : TimedXyzData(), tp_(0), st_(FingerStateNotPresent) {}
    
    /**
     * Constructor. Provide initial values for all members.
     */
    TouchData(TimedXyzData timedXyzData, int tp, FingerState st) : 
        TimedXyzData(timedXyzData), tp_(tp), st_(st) {}
};

#endif // TOUCHDATA_H
