/**
   @file contextsensor.h
   @brief ContextSensor
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

#ifndef CONTEXT_SENSOR_CHANNEL_H
#define CONTEXT_SENSOR_CHANNEL_H

#include "abstractsensor.h"
#include "orientationbin.h"
#include "compassbin.h"

class ContextSensorChannel : public AbstractSensorChannel
{
    Q_OBJECT

public:
    ~ContextSensorChannel();
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        ContextSensorChannel* sc = new ContextSensorChannel(id);
        return sc;
    }

protected:
    ContextSensorChannel(const QString& id);

private:
    ContextProvider::Service service;
    OrientationBin orientationBin;
    CompassBin compassBin;
};

#endif
