/**
   @file declinationfilter.cpp
   @brief DeclinationFilter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#include <gconf/gconf-client.h>
#include "declinationfilter.h"
#include "logging.h"

#define DECLINATION_KEY "/system/osso/location/settings/magneticvariation"

DeclinationFilter::DeclinationFilter() : 
        Filter<CompassData, DeclinationFilter, CompassData>(this, &DeclinationFilter::correct),
        declinationCorrection_(*this)
{
    declinationCorrection_(0);

    g_type_init();

    loadSettings();
}

void DeclinationFilter::correct(unsigned, const CompassData* data)
{
    newOrientation = *data;
    newOrientation.degrees_ += declinationCorrection_();
    orientation = newOrientation;
    source_.propagate(1, &orientation);
}

void DeclinationFilter::loadSettings()
{
    GConfClient *client = NULL;
    int     value;
    GError *error = NULL;

    client = gconf_client_get_default();
    if (! client ) {
        sensordLogW() << "Failed to initialise GConf client.";
        return;
    }

    value = gconf_client_get_int(client, DECLINATION_KEY, &error);
    if ( error ) {
        sensordLogW() << "Failed to read value for " << DECLINATION_KEY << "from GConf.";
        return;
    }

    declinationCorrection_(value);

    g_object_unref(client);
}
