/**
   @file magnetometeradaptor-ncdk.cpp
   @brief MagnetometerAdaptor for ncdk

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>

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
#include "magnetometeradaptor-ncdk.h"
#include <QString>
#include "config.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include "datatypes/utils.h"
#include "logging.h"

MagnetometerAdaptorNCDK::MagnetometerAdaptorNCDK(const QString& id) :
        SysfsAdaptor(id, SysfsAdaptor::IntervalMode, false)
{

    intervalCompensation_ = Config::configuration()->value("magnetometer/interval_compensation", "0").toInt();
    powerStateFilePath_ = Config::configuration()->value("magnetometer/path_power_state", "").toString();
    sensAdjFilePath_ = Config::configuration()->value("magnetometer/path_sens_adjust", "").toString();
    magnetometerBuffer_ = new DeviceAdaptorRingBuffer<TimedXyzData>(128);
    setAdaptedSensor("magnetometer", "Internal magnetometer coordinates", magnetometerBuffer_);
    setDescription("Input device Magnetometer adaptor (ak897x)");

    //get sensitivity adjustment
    sensAdj = sensAdjust();
    QStringList str_adj = sensAdj.split(":");
    if (str_adj.size() == 3)
    {
        x_adj = ((QString)str_adj.at(0)).toInt();
        y_adj = ((QString)str_adj.at(1)).toInt();
        z_adj = ((QString)str_adj.at(2)).toInt();
    }

}

void MagnetometerAdaptorNCDK::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    char buf[32];
    int x, y, z;
    QStringList str;
    bool isOK = read(fd, &buf, sizeof(buf)) > 0;

    switch (isOK)
    {
    case true:
        str = QString(buf).split(":");
        if (str.size() == 3)
        {
            x = adjustPos(((QString)str.at(0)).toInt(), x_adj);
            y = adjustPos(((QString)str.at(1)).toInt(), y_adj);
            z = adjustPos(((QString)str.at(2)).toInt(), z_adj);
            break;
        }
    case false:
        sensordLogW() << "Reading magnetometer error: " << strerror(errno);
    }

    sensordLogT() << "Magnetometer Reading: " << x << ", " << y << ", " << z;

    TimedXyzData* sample = magnetometerBuffer_->nextSlot();

    sample->timestamp_ = Utils::getTimeStamp();
    sample->x_ = x;
    sample->y_ = y;
    sample->z_ = z;

    magnetometerBuffer_->commit();
    magnetometerBuffer_->wakeUpReaders();
}

int MagnetometerAdaptorNCDK::PoserState()
{
    char buf[16];
    QString str = SysfsAdaptor::readFromFile(powerStateFilePath_, buf);
    return str.isEmpty() ? 0 : str.toUInt();
}

bool MagnetometerAdaptorNCDK::setPowerState(const int value)
{
    sensordLogD() << "Setting power state for compass driver" << " to " << value;

    QString powerStateStr = QString(value);

    if (!writeToFile(powerStateFilePath_, powerStateStr))
    {
        sensordLogW() << "Unable to set power state for compass driver";
        return false;
    }

    return true;
}

QString MagnetometerAdaptorNCDK::sensAdjust()
{
    char buf[32];
    return readFromFile(sensAdjFilePath_, buf);
}

int MagnetometerAdaptorNCDK::adjustPos(const int value, const int adj) const
{
    return value * ( adj + 128 ) / 256;
}

bool MagnetometerAdaptorNCDK::startSensor()
{
    setPowerState(1);
    return SysfsAdaptor::startSensor();
}

void MagnetometerAdaptorNCDK::stopSensor()
{
    setPowerState(0);
    SysfsAdaptor::stopSensor();
}

bool MagnetometerAdaptorNCDK::setInterval(const unsigned int value, const int sessionId)
{
    if(intervalCompensation_)
    {
        return SysfsAdaptor::setInterval(value > intervalCompensation_ ? value - intervalCompensation_ : 0, sessionId);
    }
    return SysfsAdaptor::setInterval(value, sessionId);
}
