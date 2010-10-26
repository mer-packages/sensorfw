/**
   @file samplesensor_a.h
   @brief Sample D-Bus adaptor for sensors

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

#ifndef SAMPLE_SENSOR_H
#define SAMPLE_SENSOR_H

#include <QtDBus/QtDBus>

#include "abstractsensor_a.h"
#include "datatypes/unsigned.h"

class SampleSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT;
    Q_CLASSINFO("D-Bus Interface", "local.SampleSensor")

public:
    Q_PROPERTY(Unsigned value READ get);
    Unsigned get() const;

    SampleSensorChannelAdaptor(QObject* parent);

Q_SIGNALS:
    void dataAvailable(const TimedUnsigned& data);
};

#endif
