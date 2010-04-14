/**
   @file proximitysensor_a.h
   @brief D-Bus adaptor for ProximitySensor

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

#ifndef PROXIMITY_SENSOR_H
#define PROXIMITY_SENSOR_H

#include <QtDBus/QtDBus>

#include "abstractsensor_a.h"
#include "datatypes/unsigned.h"

class ProximitySensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.ProximitySensor")

public:
    Q_PROPERTY(Unsigned proximity READ proximity);
    Unsigned proximity() const;

    ProximitySensorChannelAdaptor(QObject* parent);

Q_SIGNALS:
    void dataAvailable(const Unsigned data);
};

#endif
