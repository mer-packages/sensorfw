/**
   @file gyroscopesensor_a.h
   @brief D-Bus adaptor for GyroscopeSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>
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

#ifndef GYROSCOPE_SENSOR_H
#define GYROSCOPE_SENSOR_H

#include <QtDBus/QtDBus>

#include "abstractsensor_a.h"
#include "datatypes/orientationdata.h"
#include "datatypes/xyz.h"

class GyroscopeSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_DISABLE_COPY(GyroscopeSensorChannelAdaptor)
    Q_CLASSINFO("D-Bus Interface", "local.GyroscopeSensor")
    Q_PROPERTY(XYZ value READ value)

public:
    GyroscopeSensorChannelAdaptor(QObject* parent);

public Q_SLOTS:
    XYZ value() const;

Q_SIGNALS:
    void dataAvailable(const XYZ& data);
};

#endif
