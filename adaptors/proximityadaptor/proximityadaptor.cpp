/**
   @file proximityadaptor.cpp
   @brief ProximityAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
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
#include "proximityadaptor.h"
#include <errno.h>
#include "datatypes/utils.h"
#include "logging.h"
#include "config.h"
#include <QFile>
#include <linux/types.h>

struct bh1770glc_ps {
    __u8 led1;
    __u8 led2;
    __u8 led3;
} __attribute__((packed));


struct apds990x_data {
        __u32 lux; /* 10x scale */
        __u32 lux_raw; /* 10x scale */
        __u16 ps;
        __u16 ps_raw;
        __u16 status;
} __attribute__((packed));

ProximityAdaptor::ProximityAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::SelectMode, false)
{

#ifdef SENSORFW_MCE_WATCHER
    dbusIfc_ = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF,
                                                 QDBusConnection::systemBus(), this);
#endif

    deviceType_ = (DeviceType)Config::configuration()->value("proximity/driver_type", "0").toInt();
    threshold_ = Config::configuration()->value("proximity/threshold", "35").toInt();
    powerStatePath_ = Config::configuration()->value("proximity/powerstate_path").toByteArray();
    if (deviceType_ == RM696)
    {
#ifdef SENSORFW_MCE_WATCHER
        dbusIfc_->call(QDBus::NoBlock, "req_proximity_sensor_enable");
#endif
    }
    proximityBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(1);
    setAdaptedSensor("proximity", "Proximity state", proximityBuffer_);
    setDescription("Proximity sensor readings (Dipro sensor)");
}

ProximityAdaptor::~ProximityAdaptor()
{
#ifdef SENSORFW_MCE_WATCHER
    if(dbusIfc_)
    {
        dbusIfc_->call(QDBus::NoBlock, "req_proximity_sensor_disable");
        delete dbusIfc_;
    }
#endif
    delete proximityBuffer_;
}

bool ProximityAdaptor::startSensor()
{
    if(deviceType_ == NCDK && !powerStatePath_.isEmpty())
    {
        writeToFile(powerStatePath_, "1");
    }
    return SysfsAdaptor::startSensor();
}

void ProximityAdaptor::stopSensor()
{
    if(deviceType_ == NCDK && !powerStatePath_.isEmpty())
    {
        writeToFile(powerStatePath_, "0");
    }
    SysfsAdaptor::stopSensor();
}

void ProximityAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    int value = 0;

    if (deviceType_ == RM680)
    {
        bh1770glc_ps ps_data;
        int bytesRead = read(fd, buffer, sizeof(buffer) - 1);

        if (bytesRead > 0) {
            memcpy(&ps_data, buffer, sizeof(ps_data));
            sensordLogT() << "Proximity Values: " << ps_data.led1 << ", " << ps_data.led2 << ", " <<  ps_data.led3;
        } else {
            sensordLogW() << "read(): " << strerror(errno);
            return;
        }

        if ( ps_data.led1 > threshold_ ) {
            value = 1;
        }
    }
    else if(deviceType_ == RM696)
    {
        apds990x_data ps_data;
        int bytesRead = read(fd, buffer, sizeof(buffer) - 1);

        if (bytesRead > 0) {
            memcpy(&ps_data, buffer, sizeof(ps_data));
            sensordLogT() << "Proximity Values: " << ps_data.ps << ", " << ps_data.ps_raw << ", " << ps_data.status;
        } else {
            sensordLogW() << "read(): " << strerror(errno);
            return;
        }

        if ( ps_data.ps > threshold_ ) {
            value = 1;
        }
    }
    else if(deviceType_ == NCDK)
    {
        int bytesRead = read(fd, &buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            sensordLogW() << "read(): " << strerror(errno);
            return;
        }
        QVariant variant(buffer);
        bool ok;
        value = variant.toInt(&ok);
        if(!ok) {
            sensordLogW() << "read(): failed to parse int from: " << buffer;
            return;
        }
        sensordLogT() << "Proximity value: " << value;
    }
    else
    {
        sensordLogW() << "Not known device type: " << deviceType_;
        return;
    }

    TimedUnsigned* proximityData = proximityBuffer_->nextSlot();

    proximityData->timestamp_ = Utils::getTimeStamp();
    proximityData->value_ = value;

    proximityBuffer_->commit();
    proximityBuffer_->wakeUpReaders();
}
