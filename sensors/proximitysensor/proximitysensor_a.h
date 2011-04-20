/**
   @file proximitysensor_a.h
   @brief D-Bus adaptor for ProximitySensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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
#include "datatypes/proximity.h"

class ProximitySensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_DISABLE_COPY(ProximitySensorChannelAdaptor)
    Q_CLASSINFO("D-Bus Interface", "local.ProximitySensor")
    Q_PROPERTY(Unsigned proximity READ proximity)
    Q_PROPERTY(Proximity proximityReflectance READ proximityReflectance)

public:
    ProximitySensorChannelAdaptor(QObject* parent);

public Q_SLOTS:
    Unsigned proximity() const;
    Proximity proximityReflectance() const;

Q_SIGNALS:
    void dataAvailable(const Unsigned& data);
};

#endif
