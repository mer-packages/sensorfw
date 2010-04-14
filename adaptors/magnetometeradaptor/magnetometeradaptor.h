/**
   @file magnetometeradaptor.h
   @brief MagnetometerAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#ifndef MAGNETOMETERADAPTOR_H
#define MAGNETOMETERADAPTOR_H

#include "inputdevadaptor.h"
#include "sensord/deviceadaptorringbuffer.h"
#include "filters/orientationdata.h"
#include <QTime>

/**
 * @brief Adaptor for internal magnetometer.
 *
 * Uses Input Device system as driver interface. Measures values from the magnetometer
 * with sysfsadaptor SelectMode.
 *
 */
class MagnetometerAdaptor : public InputDevAdaptor
{
    Q_OBJECT;
public:
    /**
     * Factory method for gaining a new instance of MagnetometerAdaptor class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new MagnetometerAdaptor(id);
    }

protected:
    /**
     * Constructor.
     * @param id Identifier for the adaptor.
     */
    MagnetometerAdaptor(const QString& id);
    ~MagnetometerAdaptor();

private:

    void interpretEvent(int src, struct input_event *ev);
    void commitOutput();
    void interpretSync(int src);

    QTime time;
    DeviceAdaptorRingBuffer<MagnetometerData>* magnetometerBuffer_;
    MagnetometerData magnetometerValue_;
    int originalPollingRate_;
};

#endif

