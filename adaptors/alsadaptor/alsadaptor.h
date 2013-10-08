/**
   @file alsadaptor.h
   @brief ALSAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Shenghua <ext-shenghua.1.liu@nokia.com>
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

#ifndef ALSADAPTOR_H
#define ALSADAPTOR_H

#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/timedunsigned.h"
#include <QTime>
#include <QDBusInterface>

#ifdef SENSORFW_MCE_WATCHER
// these come from mce/mode-names.h
// and mce/dbus-names.h
#define MCE_SERVICE                     "com.nokia.mce"
#define MCE_REQUEST_PATH                "/com/nokia/mce/request"
#define MCE_REQUEST_IF                  "com.nokia.mce.request"
#endif

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
        RM696,
        NCDK
    };

    /**
     * Factory method for gaining a new instance of ALSAdaptor class.
     *
     * @param id Identifier for the adaptor.
     * @return new instance
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new ALSAdaptor(id);
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
    ALSAdaptor(const QString& id);

    ~ALSAdaptor();

    void processSample(int pathId, int fd);

private:
#ifdef SENSORFW_MCE_WATCHER
    void enableALS();
    void disableALS();

    QDBusInterface *dbusIfc;
    bool alsEnabled;
#endif

    DeviceAdaptorRingBuffer<TimedUnsigned>* alsBuffer_;
    DeviceType deviceType_;
    QByteArray powerStatePath_;
};

#endif
