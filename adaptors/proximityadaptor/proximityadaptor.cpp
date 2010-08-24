/**
   @file proximityadaptor.cpp
   @brief ProximityAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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
#include "filters/utils.h"
#include "sensord/logging.h"
#include "sensord/config.h"
#include <linux/types.h>
#include <QFile>

#define THRESHOLD_FILE_PATH "/sys/class/misc/bh1770glc_ps/device/ps_threshold"

struct bh1770glc_ps {
    __u8 led1;
    __u8 led2;
    __u8 led3;
} __attribute__((packed));

ProximityAdaptor::ProximityAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::SelectMode, Config::configuration()->value("proximity_dev_path").toString()),
    m_threshold(35)
{
    proximityBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(16);
    addAdaptedSensor("proximity", "Proximity state", proximityBuffer_);

    m_threshold = readThreshold();
    if (m_threshold <= 0) {
        sensordLogW() << "Received value 0 for proximity threshold. Falling back to default (35)";
        m_threshold = 35;
    }

    introduceAvailableDataRange(DataRange(0, 1, 1));
}

ProximityAdaptor::~ProximityAdaptor()
{
    delete proximityBuffer_;
}

void ProximityAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);
    
    static char buffer[100];
    int ret = 0;
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
        thresholdFile.setFileName(THRESHOLD_FILE_PATH);
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
