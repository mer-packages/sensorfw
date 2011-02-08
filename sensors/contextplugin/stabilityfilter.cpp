/**
   @file stabilityfilter.cpp
   @brief StabilityFilter
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>

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

#include "stabilityfilter.h"
#include "logging.h"
#include "config.h"

const int StabilityFilter::defaultTimeout = 60; // seconds

StabilityFilter::StabilityFilter(Property* stableProperty, Property* unstableProperty,
                                 double lowThreshold, double highThreshold, double hysteresis)
    : Filter<QPair<double, double>, StabilityFilter, QPair<double, double> >(this, &StabilityFilter::interpret),
      lowThreshold(lowThreshold),
      highThreshold(highThreshold),
      hysteresis(hysteresis),
      stableProperty(stableProperty),
      unstableProperty(unstableProperty)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeoutTriggered()));
    timeout = Config::configuration()->value("context/stability_timeout", QVariant(defaultTimeout)).toInt() * 1000;
}

void StabilityFilter::interpret(unsigned, const QPair<double, double>* data)
{
    // To take into account hysteresis and keep it simple, compute
    // stability and instability separately
    if (data->second < lowThreshold * (1 - hysteresis)) {
        stableProperty->setValue(true);
        timer.stop();
    }
    else {
        timer.start(timeout);

        if (data->second > lowThreshold * (1 + hysteresis)) {
            stableProperty->setValue(false);
        }
    }

    if (data->second < highThreshold * (1 - hysteresis)) {
        unstableProperty->setValue(false);
    }
    else if (data->second > highThreshold * (1 + hysteresis)) {
        unstableProperty->setValue(true);
    }

    // Propagate the data further without changing it
    source_.propagate(1, data);
}

void StabilityFilter::timeoutTriggered()
{
    sensordLogT() << "Stationary timeout triggered.";

    stableProperty->setValue(true);
    timer.stop();
}
