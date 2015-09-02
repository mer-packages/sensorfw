/**
   @file accelerometeradaptor.h
   @brief Contains AccelerometerAdaptor.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   This file is part of Sensord.

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#ifndef ACCELEROMETERADAPTOR_H
#define ACCELEROMETERADAPTOR_H

#include "inputdevadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/orientationdata.h"

/**
 * @brief Adaptor for internal accelerometer.
 *
 * Adaptor for internal accelerometer. Uses SysFs driver interface in interval
 * polling mode, i.e. values are read with given constant interval.
 *
 * Driver interface is located in @e /sys/class/i2c-adapter/i2c-3/3-001d/ .
 * <ul><li>@e coord filehandle provides measurement values.</li></ul>
 * No other filehandles are currently in use by this adaptor.
 *
 */
class AccelerometerAdaptor : public InputDevAdaptor
{
    Q_OBJECT;
public:
    /**
     * Factory method for gaining a new instance of AccelerometerAdaptor class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new AccelerometerAdaptor(id);
    }

    virtual bool startSensor();

    virtual void stopSensor();
    virtual bool standby();
    virtual bool resume();

protected:
    /**
     * Constructor.
     * @param id Identifier for the adaptor.
     */
    AccelerometerAdaptor(const QString& id);
    ~AccelerometerAdaptor();

    /**
     * Reimplement to allow for 0 interval to be the slowest entry.
     */
    virtual unsigned int evaluateIntervalRequests(int& sessionId) const;

private:
    DeviceAdaptorRingBuffer<AccelerationData>* accelerometerBuffer_;
    AccelerationData orientationValue_;

    void interpretEvent(int src, struct input_event *ev);
    void commitOutput(struct input_event *ev);
    void interpretSync(int src, struct input_event *ev);
    QByteArray powerStatePath_;
};

#endif
