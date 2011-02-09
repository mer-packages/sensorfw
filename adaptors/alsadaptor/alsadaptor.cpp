/**
   @file alsadaptor.cpp
   @brief ALSAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
   @author Tapio Rantala <ext-tapio.rantala@nokia.com>
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

#include "logging.h"
#include "config.h"
#include "alsadaptor.h"
#include <errno.h>
#include "datatypes/utils.h"
#include <linux/types.h>
#include <QFile>

/* Device name: /dev/apds990x0 */
struct apds990x_data {
    __u32 lux; /* 10x scale */
    __u32 lux_raw; /* 10x scale */
    __u16 ps;
    __u16 ps_raw;
    __u16 status;
} __attribute__((packed));

struct bh1770glc_als {
    __u16 lux;
} __attribute__((packed));


ALSAdaptor::ALSAdaptor(const QString& id):
    SysfsAdaptor(id, SysfsAdaptor::SelectMode, false)
{
    alsBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(32);
    setAdaptedSensor("als", "Internal ambient light sensor lux values", alsBuffer_);
    setDescription("Ambient light");
    deviceType_ = (DeviceType)Config::configuration()->value("als/driver_type", "0").toInt();
    powerStatePath_ = Config::configuration()->value("als/powerstate_path").toByteArray();
}

ALSAdaptor::~ALSAdaptor()
{
    delete alsBuffer_;
}

bool ALSAdaptor::startSensor()
{
    if(deviceType_ == NCDK && !powerStatePath_.isEmpty())
    {
        writeToFile(powerStatePath_, "1");
    }
    return SysfsAdaptor::startSensor();
}

void ALSAdaptor::stopSensor()
{
    if(deviceType_ == NCDK && !powerStatePath_.isEmpty())
    {
        writeToFile(powerStatePath_, "0");
    }
    SysfsAdaptor::stopSensor();
}

void ALSAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    if(deviceType_ == RM680)
    {
        struct bh1770glc_als als_data;
        als_data.lux = 0;

        int bytesRead = read(fd, &als_data, sizeof(als_data));

        if (bytesRead <= 0) {
            sensordLogW() << "read(): " << strerror(errno);
            return;
        }
        sensordLogT() << "Ambient light value: " << als_data.lux;

        TimedUnsigned* lux = alsBuffer_->nextSlot();
        lux->value_ = als_data.lux;
        lux->timestamp_ = Utils::getTimeStamp();
    }
    else if (deviceType_ == RM696)
    {
        struct apds990x_data als_data;
        als_data.lux = 0;

        int bytesRead = read(fd, &als_data, sizeof(als_data));

        if (bytesRead <= 0) {
            sensordLogW() << "read(): " << strerror(errno);
            return;
        }
        sensordLogT() << "Ambient light value: " << als_data.lux;

        TimedUnsigned* lux = alsBuffer_->nextSlot();
        lux->value_ = als_data.lux;
        lux->timestamp_ = Utils::getTimeStamp();
    }
    else if (deviceType_ == NCDK)
    {
        char buffer[32];
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(fd, &buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            sensordLogW() << "read(): " << strerror(errno);
            return;
        }
        QVariant value(buffer);
        bool ok;
        double fValue(value.toDouble(&ok));
        if(!ok) {
            sensordLogW() << "read(): failed to parse float from: " << buffer;
            return;
        }
        TimedUnsigned* lux = alsBuffer_->nextSlot();
        lux->value_ = fValue * 10;
        lux->timestamp_ = Utils::getTimeStamp();
        sensordLogT() << "Ambient light value: " << lux->value_;
    }
    else
    {
        sensordLogW() << "Not known device type: " << deviceType_;
        return;
    }
    alsBuffer_->commit();
    alsBuffer_->wakeUpReaders();
}
