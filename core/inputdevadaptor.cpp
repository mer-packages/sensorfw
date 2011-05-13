/**
   @file inputdevadaptor.cpp
   @brief Base class for input layer device adaptors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
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

#include "inputdevadaptor.h"
#include "config.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <QFile>
#include <QDir>
#include <QString>

InputDevAdaptor::InputDevAdaptor(const QString& id, int maxDeviceCount) :
    SysfsAdaptor(id, SysfsAdaptor::SelectMode, false),
    deviceCount_(0),
    maxDeviceCount_(maxDeviceCount),
    cachedInterval_(0)
{
    memset(evlist_, 0x0, sizeof(input_event)*64);
}

InputDevAdaptor::~InputDevAdaptor()
{
}

int InputDevAdaptor::getInputDevices(const QString& typeName)
{
    QString deviceSysPathString = Config::configuration()->value("global/device_sys_path").toString();
    QString devicePollFilePath = Config::configuration()->value("global/device_poll_file_path").toString();

    int deviceNumber = 0;
    deviceString_ = typeName;

    // Check if this device name is defined in configuration
    QString deviceName = Config::configuration()->value<QString>(typeName + "/device", "");

    // Do not perform strict checks for the input device
    if (deviceName.size() && checkInputDevice(deviceName, typeName, false)) {
        addPath(deviceName, deviceCount_);
        ++deviceCount_;
    }
    else if(deviceSysPathString.contains("%1"))
    {
        const int MAX_EVENT_DEV = 16;

        // No configuration for this device, try find the device from the device system path
        while (deviceNumber < MAX_EVENT_DEV && deviceCount_ < maxDeviceCount_) {
            deviceName = deviceSysPathString.arg(deviceNumber);
            if (checkInputDevice(deviceName, typeName)) {
                addPath(deviceName, deviceCount_);
                ++deviceCount_;
                break;
            }
            ++deviceNumber;
        }
    }

    QString pollConfigKey = QString(typeName + "/poll_file");
    if (Config::configuration()->exists(pollConfigKey)) {
        usedDevicePollFilePath_ = Config::configuration()->value<QString>(pollConfigKey, "");
    } else {
        usedDevicePollFilePath_ = devicePollFilePath.arg(deviceNumber);
    }

    if (deviceCount_ == 0) {
        sensordLogW() << "Cannot find any device for: " << typeName;
        setValid(false);
    }
    else
    {
        QByteArray byteArray = readFromFile(usedDevicePollFilePath_.toAscii());
        cachedInterval_ = byteArray.size() > 0 ? byteArray.toInt() : 0;
    }

    return deviceCount_;
}

int InputDevAdaptor::getEvents(int fd)
{
    int bytes = read(fd, evlist_, sizeof(struct input_event)*64);
    if (bytes == -1) {
        sensordLogW() << "Error occured: " << strerror(errno);
        return 0;
    }
    if (bytes % sizeof(struct input_event)) {
        sensordLogW() << "Short read or stray bytes.";
        return 0;
    }
    return bytes/sizeof(struct input_event);
}

void InputDevAdaptor::processSample(int pathId, int fd)
{
    int numEvents = getEvents(fd);

    for (int i = 0; i < numEvents; ++i) {
        switch (evlist_[i].type) {
            case EV_SYN:
                interpretSync(pathId, &(evlist_[i]));
                break;
            default:
                interpretEvent(pathId, &(evlist_[i]));
                break;
        }
    }
}

bool InputDevAdaptor::checkInputDevice(const QString& path, const QString& matchString, bool strictChecks) const
{
    char deviceName[256] = {0,};
    bool check = true;

    int fd = open(path.toLocal8Bit().constData(), O_RDONLY);
    if (fd == -1) {
        return false;
    }

    if (strictChecks) {
        int result = ioctl(fd, EVIOCGNAME(sizeof(deviceName)), deviceName);
        if (result == -1) {
           sensordLogW() << "Could not read devicename for " << path;
           check = false;
        } else
        if (QString(deviceName).contains(matchString, Qt::CaseInsensitive)) {
            sensordLogT() << "\"" << matchString << "\"" << " matched in device name: " << deviceName;
            check = true;
        } else {
            check = false;
        }
    }

    close(fd);

    return check;
}

unsigned int InputDevAdaptor::interval() const
{
    return cachedInterval_;
}

bool InputDevAdaptor::setInterval(const unsigned int value, const int sessionId)
{
    Q_UNUSED(sessionId);

    sensordLogD() << "Setting poll interval for " << deviceString_ << " to " << value;
    QByteArray frequencyString(QString("%1\n").arg(value).toLocal8Bit());
    if(writeToFile(usedDevicePollFilePath_.toLocal8Bit(), frequencyString))
    {
        cachedInterval_ = value;
        return true;
    }
    return false;
}

void InputDevAdaptor::init()
{
    if (!getInputDevices(Config::configuration()->value<QString>(name() + "/input_match", name()))) {
        sensordLogW() << "Input device not found.";
    }
    SysfsAdaptor::init();
}

int InputDevAdaptor::getDeviceCount() const
{
    return deviceCount_;
}
