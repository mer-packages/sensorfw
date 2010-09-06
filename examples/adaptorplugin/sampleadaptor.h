/**
   @file sampleadaptor.h
   @brief SampleAdaptor based on SysfsAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#ifndef SAMPLEADAPTOR_H
#define SAMPLEADAPTOR_H

// Include any parent classes this adaptor is based on
// (deviceadaptor really not necessary, as it's a parent of sysfs)
#include "sensord/deviceadaptor.h"
#include "sensord/sysfsadaptor.h"

// Include any datatypes this adaptor produces. For example, let's use
// TimedUnsigned (unsigned int + timestamp)
#include "datatypes/timedunsigned.h"

/**
 * Adaptor description...
 */
class SampleAdaptor : public SysfsAdaptor
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
        return new SampleAdaptor(id);
    }

protected:

    /**
     * Constructor. Protected to force externals to use factory method.
     *
     * @param id Identifier for the adaptor.
     */
    SampleAdaptor(const QString& id);

    /**
     * Destructor.
     */
    ~SampleAdaptor();

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

    // A ring buffer for passing the received data on to the filtering
    // chain. Type must match the type expected by the receiving end.
    DeviceAdaptorRingBuffer<TimedUnsigned>* outputBuffer_;
};

#endif
