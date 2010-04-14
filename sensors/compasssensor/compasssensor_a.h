/**
   @file compasssensor_a.h
   @brief D-Bus adaptor for CompassSensor

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

#ifndef COMPASS_SENSOR_H
#define COMPASS_SENSOR_H

#include <QtDBus/QtDBus>
#include "abstractsensor_a.h"
#include "datatypes/compass.h"

class CompassSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.CompassSensor")

public:

    Q_PROPERTY(Compass value READ get)
    Compass get() const;

    Q_PROPERTY(bool usedeclination READ useDeclination WRITE setUseDeclination);
    bool useDeclination() const;
    void setUseDeclination(bool enable);

    Q_PROPERTY(int declinationvalue READ declinationValue);
    int declinationValue() const;

    CompassSensorChannelAdaptor(QObject* parent);

Q_SIGNALS:
    void dataAvailable(const Compass& value);
};

#endif
