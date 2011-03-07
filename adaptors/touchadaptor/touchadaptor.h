/**
   @file touchadaptor.h
   @brief TouchAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#ifndef TOUCHADAPTOR_H
#define TOUCHADAPTOR_H

#include "inputdevadaptor.h"
#include "deviceadaptorringbuffer.h"
#include <QObject>
#include "touchdata.h"

/**
 * @brief Adaptor for device touchscreen.
 *
 * Provides input data from touchscreen input device.
 */
class TouchAdaptor : public InputDevAdaptor
{
    Q_OBJECT;
public:
    /**
     * Factory method for gaining a new instance of TouchAdaptor class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new TouchAdaptor(id);
    }

protected:
    /**
     * Constructor.
     * @param id Identifier for the adaptor.
     */
    TouchAdaptor(const QString& id);
    ~TouchAdaptor();

private:

    static const int HARD_MAX_TOUCH_POINTS;

    /**
     * Holds values read from the driver.
     */
    struct TouchValues {
        int x;
        int y;
        int z;
        int volume;
        int toolWidth;
        TouchData::FingerState fingerState;
    };

    /**
     * Holds information related to screen properties.
     */
    struct RangeInfo {
        RangeInfo() : xMin(0), yMin(0), xRange(0), yRange(0) {};

        int xMin;   /**< Minimum value from the driver for X-axis */
        int yMin;   /**< Minimum value from the driver for Y-axis */
        int xRange; /**< Width of X-axis */
        int yRange; /**< Width of Y-axis */
    };

    /**
     * Verify whether the input device handle on given path is a touchscreen device.
     * @return True if yes, false if not.
     */
    bool checkInputDevice(QString path, QString matchString = "");

    /**
     * Interpret a read event and store the received value.
     * @param src Event source.
     * @param ev  Read event.
     */
    void interpretEvent(int src, struct input_event *ev);

    /**
     * Pushes recorded events into filterchain.
     * @param src Event source.
     */
    void commitOutput(int src, struct input_event *ev);

    void interpretSync(int src, struct input_event *ev);

    DeviceAdaptorRingBuffer<TouchData>* outputBuffer_;
    TouchValues touchValues_[5];
    RangeInfo rangeInfo_;
};

#endif
