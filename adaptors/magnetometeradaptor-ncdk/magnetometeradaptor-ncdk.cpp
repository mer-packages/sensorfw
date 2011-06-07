/**
   @file magnetometeradaptor-ncdk.cpp
   @brief MagnetometerAdaptor for ncdk

   <p>
   Copyright (C) 2010-2011 Nokia Corporation

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
#include "datatypes/utils.h"
#include "logging.h"

MagnetometerAdaptorNCDK::MagnetometerAdaptorNCDK(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::IntervalMode),
    powerState_(false)
{
    intervalCompensation_ = Config::configuration()->value<int>("magnetometer/interval_compensation", 0);
    powerStateFilePath_ = Config::configuration()->value<QByteArray>("magnetometer/path_power_state", "");
    sensAdjFilePath_ = Config::configuration()->value<QByteArray>("magnetometer/path_sens_adjust", "");
    magnetometerBuffer_ = new DeviceAdaptorRingBuffer<TimedXyzData>(128);
    setAdaptedSensor("magnetometer", "Internal magnetometer coordinates", magnetometerBuffer_);
    setDescription("Magnetometer adaptor (ak8975) for NCDK");

    //get sensitivity adjustment
    getSensitivityAdjustment(x_adj, y_adj, z_adj);

    overflowLimit_ = Config::configuration()->value<int>("magnetometer/overflow_limit", 8000);
}

MagnetometerAdaptorNCDK::~MagnetometerAdaptorNCDK()
{
    delete magnetometerBuffer_;
}

void MagnetometerAdaptorNCDK::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    if (!powerState_)
        return;

    char buf[32];
    int x = 0, y = 0, z = 0;

    QList<QByteArray> strList;
    int bytesRead = 0;
    bool isOK = (bytesRead = read(fd, &buf, sizeof(buf))) > 0;

    switch (isOK)
    {
    case true:
        strList = QByteArray(buf, bytesRead).split(':');
        if (strList.size() == 3)
        {
            x = adjustPos(strList.at(0).toInt(), x_adj);
            y = adjustPos(strList.at(1).toInt(), y_adj);
            z = adjustPos(strList.at(2).toInt(), z_adj);
            break;
        }
    case false:
        sensordLogW() << "Reading magnetometer error: " << strerror(errno);
        return;
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

bool MagnetometerAdaptorNCDK::setPowerState(bool value) const
{
    sensordLogD() << "Setting power state for compass driver" << " to " << value;

    QByteArray powerStateStr = QByteArray::number(value);

    if (!writeToFile(powerStateFilePath_, powerStateStr))
    {
        sensordLogW() << "Unable to set power state for compass driver";
        return false;
    }
    return true;
}

void MagnetometerAdaptorNCDK::getSensitivityAdjustment(int &x, int &y, int &z) const
{
    QByteArray byteArray = readFromFile(sensAdjFilePath_);

    QList<QByteArray> strList = byteArray.split(':');
    if (strList.size() == 3)
    {
        x = strList.at(0).toInt();
        y = strList.at(1).toInt();
        z = strList.at(2).toInt();
    }
}

int MagnetometerAdaptorNCDK::adjustPos(const int value, const int adj) const
{
    return value * ( adj + 128 ) / 256;
}

bool MagnetometerAdaptorNCDK::startSensor()
{
    if (!setPowerState(true))
    {
        sensordLogW() << "Unable to set power on for compass driver";
    }
    else
    {
        powerState_ = true;
    }

    return SysfsAdaptor::startSensor();
}

void MagnetometerAdaptorNCDK::stopSensor()
{
    if (!setPowerState(false))
    {
        sensordLogW() << "Unable to set power off for compass driver";
    }
    else
    {
        powerState_ = false;
    }

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

void MagnetometerAdaptorNCDK::setOverflowLimit(int limit)
{
    overflowLimit_ = limit;
}

int MagnetometerAdaptorNCDK::overflowLimit() const
{
    return overflowLimit_;
}
