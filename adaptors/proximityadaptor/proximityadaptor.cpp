/**
   @file proximityadaptor.cpp
   @brief ProximityAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
   @author Shenghua <ext-shenghua.1.liu@nokia.com>

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

#define THRESHOLD_FILE_PATH_RM680 "/sys/class/misc/bh1770glc_ps/device/ps_threshold"
#define THRESHOLD_FILE_PATH_RM696 "/sys/class/misc/apds990x0/device/prox_threshold"

#define RM680_PS "/dev/bh1770glc_ps"
#define RM696_PS "/dev/apds990x0"

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
    SysfsAdaptor(id, SysfsAdaptor::SelectMode, false),
    m_threshold(35)
{

#ifdef SENSORFW_MCE_WATCHER
    dbusIfc = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF,
                                                 QDBusConnection::systemBus(), this);
#endif

    device = DeviceUnknown;

    QString rm680_ps = Config::configuration()->value("proximity_dev_path_rm680").toString();
    QString rm696_ps = Config::configuration()->value("proximity_dev_path_rm696").toString();

    if (QFile::exists(rm680_ps))
    {
        device = RM680;
        addPath(rm680_ps);
    } else if (QFile::exists(RM680_PS))
    {
        device = RM680;
        addPath(RM680_PS);
    } else if (QFile::exists(rm696_ps))
    {
        device = RM696;
        addPath(rm696_ps);
    } else if (QFile::exists(RM696_PS))
    {
        device = RM696;
        addPath(RM696_PS);
    }

    if (device == DeviceUnknown)
    {
        sensordLogW() << "Other HW except RM680 and RM696";
    }

    if (device == RM696)
    {
#ifdef SENSORFW_MCE_WATCHER
        dbusIfc -> call(QDBus::NoBlock, "req_proximity_sensor_enable");
#endif
    }

    proximityBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(1);
    addAdaptedSensor("proximity", "Proximity state", proximityBuffer_);

    m_threshold = readThreshold();
    if (m_threshold <= 0) {
        sensordLogW() << "Received value 0 for proximity threshold. Falling back to default (35)";
        m_threshold = 35;
    }

    setDescription("Proximity sensor readings (Dipro sensor)");
    introduceAvailableDataRange(DataRange(0, 1, 1));
    introduceAvailableInterval(DataRange(0, 0, 0));
    setDefaultInterval(0);
}


ProximityAdaptor::~ProximityAdaptor()
{
    if (device == RM696)
    {
#ifdef SENSORFW_MCE_WATCHER
        dbusIfc -> call(QDBus::NoBlock, "req_proximity_sensor_disable");
        delete dbusIfc;
#endif
    }

    delete proximityBuffer_;
}


void ProximityAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    static char buffer[100];
    int ret = 0;

    if (device == RM680)
    {
        bh1770glc_ps ps_data;
        int bytesRead = read(fd, buffer, sizeof(buffer)-1);

        if (bytesRead > 0) {
            memcpy(&ps_data, buffer, sizeof(ps_data));
            sensordLogT() << "Proximity Values: " << ps_data.led1 << ", " << ps_data.led2 << ", " <<  ps_data.led3;
        } else {
            sensordLogW() << "read():" << strerror(errno);
            return;
        }

        if ( ps_data.led1 > m_threshold ) {
            ret = 1;
        }
    } else if(device == RM696)
    {
        apds990x_data ps_data;
        int bytesRead = read(fd, buffer, sizeof(buffer)-1);

        if (bytesRead > 0) {
            memcpy(&ps_data, buffer, sizeof(ps_data));
            sensordLogT() << "Proximity Values: " << ps_data.ps << ", " << ps_data.ps_raw << ", " << ps_data.status;
        } else {
            sensordLogW() << "read():" << strerror(errno);
            return;
        }

        if ( ps_data.ps > m_threshold ) {
            ret = 1;
        }
    } else
    {
        sensordLogW() << "Other HW except RM680 and RM696";
    }


    TimedUnsigned* proximityData = proximityBuffer_->nextSlot();

    proximityData->timestamp_ = Utils::getTimeStamp();
    proximityData->value_ = ret;

    proximityBuffer_->commit();
    proximityBuffer_->wakeUpReaders();
}


int ProximityAdaptor::readThreshold()
{
    int value = 0;
    QFile thresholdFile;
    QString configKey = "proximity_threshold";

    if (Config::configuration()->value(configKey, "").toString().size() > 0) {
        thresholdFile.setFileName(Config::configuration()->value(configKey, "").toString());
    } else {
        if (device == RM680)
        {
            thresholdFile.setFileName(THRESHOLD_FILE_PATH_RM680);
        }else if (device == RM696)
        {
            thresholdFile.setFileName(THRESHOLD_FILE_PATH_RM696);
        }else
        {
        }
    }

    if (!(thresholdFile.exists() && thresholdFile.open(QIODevice::ReadOnly))) {
        sensordLogW() << "Unable to locate threshold setting for" << id();
        return value;
    }

    char buf[16];

    if (thresholdFile.readLine(buf, sizeof(buf)) > 0) {
        value = QString(buf).split(" ").at(0).toInt();
    }

    thresholdFile.close();
    return value;
}
