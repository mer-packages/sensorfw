/**
   @file alssensor_a.h
   @brief D-Bus adaptor for ALSSensor

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

#ifndef ALS_SENSOR_H
#define ALS_SENSOR_H

#include <QtDBus/QtDBus>
#include <QObject>

#include "datatypes/unsigned.h"
#include "abstractsensor_a.h"

class ALSSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_DISABLE_COPY(ALSSensorChannelAdaptor)
    Q_CLASSINFO("D-Bus Interface", "local.ALSSensor")
    Q_PROPERTY(Unsigned lux READ lux)

public:
    ALSSensorChannelAdaptor(QObject* parent);

public Q_SLOTS:
    Unsigned lux() const;

Q_SIGNALS:
    void ALSChanged(const Unsigned& value);
};

#endif
