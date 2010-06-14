/**
   @file proximityadaptor.h
   @brief ProximityAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#ifndef PROXIMITYADAPTOR_H
#define PROXIMITYADAPTOR_H

#include "sysfsadaptor.h"
#include "sensord/deviceadaptorringbuffer.h"
#include "filters/timedunsigned.h"

/**
 * @brief Adaptor for proximity sensor.
 *
 * Adaptor for proximity. Uses SysFs driver interface in interval
 * polling mode, i.e. values are read with given constant interval.
 *
 */
class ProximityAdaptor : public SysfsAdaptor
{
    Q_OBJECT;
public:
    /**
     * Factory method for gaining a new instance of ProximityAdaptor class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new ProximityAdaptor(id);
    }

protected:
    /**
     * Constructor.
     * @param id Identifier for the adaptor.
     */
    ProximityAdaptor(const QString& id);
    ~ProximityAdaptor();

private:
    DeviceAdaptorRingBuffer<TimedUnsigned>* proximityBuffer_;

    /**
     * Read and process data. Run when sysfsadaptor has detected new available
     * data.
     * @param pathId PathId for the file that had event. Always 0, as we monitor
     *               only single file and don't set any proper id.
     * @param fd     Open file descriptor with new data. See #SysfsAdaptor::processSample()
     */
    void processSample(int pathId, int fd);

    /**
     * Reads and returns the correct threshold value for proximity off.
     * Any values below threshold should be considered off values for
     * proximity.
     *
     * @return Threshold value for proximity. \c 0 on error.
     */
    int readThreshold();

    int m_threshold;
};

#endif
