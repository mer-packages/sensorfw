/**
   @file compasssensor_a.h
   @brief D-Bus adaptor for CompassSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#ifndef COMPASS_SENSOR_H
#define COMPASS_SENSOR_H

#include <QtDBus/QtDBus>
#include "abstractsensor_a.h"
#include "datatypes/compass.h"

class CompassSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_DISABLE_COPY(CompassSensorChannelAdaptor)
    Q_CLASSINFO("D-Bus Interface", "local.CompassSensor")
    Q_PROPERTY(Compass value READ value)
    Q_PROPERTY(bool useDeclination READ useDeclination WRITE setUseDeclination)
    Q_PROPERTY(int declinationValue READ declinationValue)

public:
    CompassSensorChannelAdaptor(QObject* parent);

public Q_SLOTS:
    Compass value() const;
    bool useDeclination() const;
    void setUseDeclination(bool enable);
    int declinationValue() const;

Q_SIGNALS:
    void dataAvailable(const Compass& value);
};

#endif
