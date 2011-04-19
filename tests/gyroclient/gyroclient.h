/**
   @file gyroclient.h
   @brief Manual test for gyroscope client interfaces

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Samuli Piippo <ext-samuli.1.piippo@nokia.com>

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

#include "gyroscopesensor_i.h"
#include "sensormanagerinterface.h"
#include "datatypes/xyz.h"

class GyroClient : public QObject
{
    Q_OBJECT;

public:
    GyroClient();
    virtual ~GyroClient();


private slots:
    void slotDataAvailable(const XYZ& data);

private:
    static SensorManagerInterface           *m_remoteSensorManager;
    GyroscopeSensorChannelInterface         *m_sensorIfc;
};
