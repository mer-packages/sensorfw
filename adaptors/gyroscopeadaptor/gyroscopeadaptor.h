/**
   @file gyroscopeadaptor.h
   @brief GyroscopeAdaptor based on SysfsAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#ifndef GYROSCOPEADAPTOR_H
#define GYROSCOPEADAPTOR_H

#include "sysfsadaptor.h"
#include "datatypes/orientationdata.h"

/**
 * @brief Adaptor for internal gyroscope.
 *
 * Adaptor for internal gyroscope. Uses SysFs driver interface in
 * polling mode, i.e. values are read with given constant interval.
 *
 * Driver interface is located in @e /sys/class/i2c-adapter/?????? .
 * <ul><li>@e angular_rate filehandle provides measurement values.</li></ul>
 * No other filehandles are currently in use by this adaptor.
 */
class GyroscopeAdaptor : public SysfsAdaptor
{
    Q_OBJECT;
public:
    /**
     * Factory method for gaining a new instance of this adaptor class.
     *
     * @param id Identifier for the adaptor.
     * @return A pointer to created adaptor with base class type.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new GyroscopeAdaptor(id);
    }

protected:

    /**
     * Constructor. Protected to force externals to use factory method.
     *
     * @param id Identifier for the adaptor.
     */
    GyroscopeAdaptor(const QString& id);

    /**
     * Destructor.
     */
    ~GyroscopeAdaptor();

    bool setInterval(const unsigned int value, const int sessionId);
    unsigned int interval() const;

private:

    /**
     * Read and process data. Run when sysfsadaptor has detected new
     * available data.
     *
     * @param pathId PathId for the file that had event.
     * @param fd Open file descriptor with new data. See
     *           #SysfsAdaptor::processSample()
     */
    void processSample(int pathId, int fd);

    DeviceAdaptorRingBuffer<TimedXyzData>* gyroscopeBuffer_;
    QByteArray dataRatePath_;
};

#endif
