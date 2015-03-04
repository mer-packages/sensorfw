/**
   @file gyroevdevadaptor.h
   @brief Contains GyroAdaptorEvdev.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2015 Jolla

   @author Lorn Potter <lorn.potter@jolla.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Markus Lehtonen <markus.lehtonen@nokia.com>
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
#ifndef GYROEVDEVADAPTOR_H
#define GYROEVDEVADAPTOR_H

#include "inputdevadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/orientationdata.h"
#include <QTime>

class GyroAdaptorEvdev : public InputDevAdaptor
{
    Q_OBJECT
public:
    /**
     * Factory method for gaining a new instance of AccelerometerAdaptor class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new GyroAdaptorEvdev(id);
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
    GyroAdaptorEvdev(const QString& id);
    ~GyroAdaptorEvdev();

    /**
     * Reimplement to allow for 0 interval to be the slowest entry.
     */
    virtual unsigned int evaluateIntervalRequests(int& sessionId) const;

private:
    DeviceAdaptorRingBuffer<TimedXyzData>* gyroscopeBuffer_;

    void interpretEvent(int src, struct input_event *ev);
    void commitOutput(struct input_event *ev);
    void interpretSync(int src, struct input_event *ev);
    QByteArray powerStatePath_;
    TimedXyzData gyroValue_;

};

#endif
