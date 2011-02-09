/**
   @file proximityadaptor.h
   @brief ProximityAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
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

#ifndef PROXIMITYADAPTOR_H
#define PROXIMITYADAPTOR_H

#include <QDBusInterface>
#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/timedunsigned.h"

#ifdef SENSORFW_MCE_WATCHER
#include <mce/mode-names.h>
#include <mce/dbus-names.h>
#endif

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

    enum DeviceType
    {
        DeviceUnknown = 0,
        RM680,
        RM696,
        NCDK
    };

    /**
     * Factory method for gaining a new instance of ProximityAdaptor class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new ProximityAdaptor(id);
    }

    virtual bool startSensor();

    virtual void stopSensor();

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

    int threshold_;
    ProximityAdaptor::DeviceType deviceType_;
    QByteArray powerStatePath_;

#ifdef SENSORFW_MCE_WATCHER
    QDBusInterface *dbusIfc_;
#endif

};

#endif
