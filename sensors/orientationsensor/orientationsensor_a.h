/**
   @file orientationsensor_a.h
   @brief D-Bus Adaptor for OrientationSensor

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

#ifndef ORIENTATION_SENSOR_H
#define ORIENTATION_SENSOR_H

#include <QtDBus/QtDBus>

#include "datatypes/orientation.h"
#include "datatypes/unsigned.h"
#include "abstractsensor_a.h"

class OrientationSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.OrientationSensor")

public:
    Q_PROPERTY(Unsigned orientation READ orientation)
    Unsigned orientation() const;

    Q_PROPERTY(int threshold READ threshold WRITE setThreshold)
    int threshold() const;
    void setThreshold(int value);

    OrientationSensorChannelAdaptor(QObject* parent);

Q_SIGNALS:
    void orientationChanged(const Unsigned& orientation);
};

#endif
