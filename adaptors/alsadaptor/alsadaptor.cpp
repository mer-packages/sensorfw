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

#define RM680_ALS "/dev/bh1770glc_als"
#define RM696_ALS "/dev/apds990x0"

#define APDS990X_ALS_SATURATED  0x1 /* ADC overflow. result unreliable */
#define APDS990X_PS_ENABLED     0x2 /* Proximity sensor active */
#define APDS990X_ALS_UPDATED    0x4 /* ALS result updated in the response */
#define APDS990X_PS_UPDATED     0x8 /* Prox result updated in the response */

#define APDS990X_ALS_OUTPUT_SCALE 10

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
    device = DeviceUnknown;

    QString rm680_als = Config::configuration()->value("als_dev_path_rm680").toString();
    QString rm696_als = Config::configuration()->value("als_dev_path_rm696").toString();

    if (QFile::exists(rm680_als))
    {
        device = RM680;
        addPath(rm680_als);
    } else if (QFile::exists(RM680_ALS))
    {
        device = RM680;
        addPath(RM680_ALS);
    } else if (QFile::exists(rm696_als))
    {
        device = RM696;
        addPath(rm696_als);
    } else if (QFile::exists(RM696_ALS))
    {
        device = RM696;
        addPath(RM696_ALS);
    }

    if (device == DeviceUnknown)
    {
        sensordLogW() << "Other HW except RM680 and RM696";
    }

    alsBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(32);
    addAdaptedSensor("als", "Internal ambient light sensor lux values", alsBuffer_);

    setDescription("Ambient light");
    introduceAvailableDataRange(DataRange(0, 65535, 1));
    introduceAvailableInterval(DataRange(0, 0, 0));
    setDefaultInterval(0);

}


ALSAdaptor::~ALSAdaptor()
{
    delete alsBuffer_;
}


void ALSAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    if(device == RM680)
    {
        struct bh1770glc_als als_data;
        als_data.lux = 0;

        int bytesRead = read(fd, &als_data, sizeof(als_data));

        if (bytesRead <= 0) {
            sensordLogW() << "read():" << strerror(errno);
            return;
        }
        sensordLogT() << "Ambient light value: " << als_data.lux;

        TimedUnsigned* lux = alsBuffer_->nextSlot();
        lux->value_ = als_data.lux;
        lux->timestamp_ = Utils::getTimeStamp();
    }

    if (device == RM696)
    {
        struct apds990x_data als_data;

        als_data.lux = 0;

        int bytesRead = read(fd, &als_data, sizeof(als_data));

        if (bytesRead <= 0) {
            sensordLogW() << "read():" << strerror(errno);
            return;
        }
        sensordLogT() << "Ambient light value: " << als_data.lux;

        TimedUnsigned* lux = alsBuffer_->nextSlot();
        lux->value_ = als_data.lux;
        lux->timestamp_ = Utils::getTimeStamp();
    }

    alsBuffer_->commit();
    alsBuffer_->wakeUpReaders();

}
