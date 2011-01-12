/**
   @file alsadaptor.h
   @brief ALSAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>

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

#ifndef ALSADAPTOR_H
#define ALSADAPTOR_H

#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/timedunsigned.h"
#include <QTime>

/**
 * @brief Adaptor for internal ambient light sensor.
 *
 * Adaptor for the internal ambient light sensor. Provides the amount of ambient
 * light detected by the device.
 *
 * Sysfs driver interface can be found at @e dev/bh1770glc_als .
 *
 * Value output frequency depends on driver decision - only changed values
 * are pushed out of driver.
 */
class ALSAdaptor : public SysfsAdaptor
{
    Q_OBJECT;
public:

    enum DeviceType
    {
        DeviceUnknown = 0,
        RM680,
        RM696
    };

    /**
     * Factory method for gaining a new instance of ALSAdaptor class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new ALSAdaptor(id);
    }

protected:
    /**
     * Constructor.
     * @param id Identifier for the adaptor.
     */
    ALSAdaptor(const QString& id);
    ~ALSAdaptor();

    /**
     * Reimplemented to prevent standbyOverride for this adaptor.
     *
     * Not provided for this adaptor due to requirement that an external
     * entity controls the driver interface for thresholds.
     *
     * @param override Attempted override value.
     * @return Always false.
     */
    virtual bool setStandbyOverride(const bool override) { Q_UNUSED(override); return false; }
private:

    /**
     * Read and process data. Run when sysfsadaptor has detected new available
     * data.
     * @param pathId PathId for the file that had event. Always 0, as we monitor
     *               only single file and don't set any proper id.
     * @param fd     Open file descriptor with new data. See #SysfsAdaptor::processSample()
     */
    void processSample(int pathId, int fd);

    DeviceAdaptorRingBuffer<TimedUnsigned>* alsBuffer_;
    ALSAdaptor::DeviceType device;

};

#endif

