/**
   @file inputdevadaptor.cpp
   @brief Base class for input layer device adaptors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
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
    deviceNumber_(-1),
    originalPollingInterval_(0)
{
    deviceSysPathString_ = Config::configuration()->value("device_sys_path").toString();
    devicePollFilePath_ = Config::configuration()->value("device_poll_file_path").toString();
}

InputDevAdaptor::~InputDevAdaptor()
{
}

int InputDevAdaptor::getInputDevices(const QString& typeName)
{
    int deviceNumber = 0;
    deviceCount_ = 0;
    deviceString_ = typeName;

    // Check if this device name is defined in configuration
    QString deviceName = Config::configuration()->value(typeName + "/device", "DEV_NOT_FOUND").toString();

    // Do not perform strict checks for the input device
    if (deviceName != "DEV_NOT_FOUND" && checkInputDevice(deviceName, typeName, false)) {
        addPath(deviceName, deviceCount_);
        ++deviceCount_;
        deviceNumber_ = deviceNumber;
    }
    else
    {
        const int MAX_EVENT_DEV = 16;

        // No configuration for this device, try find the device from the device system path
        while (deviceNumber < MAX_EVENT_DEV && deviceCount_ < maxDeviceCount_) {
            deviceName = deviceSysPathString_.arg(deviceNumber);
            if (checkInputDevice(deviceName, typeName)) {
                addPath(deviceName, deviceCount_);
                ++deviceCount_;
                deviceNumber_ = deviceNumber;
            }
            ++deviceNumber;
        }
    }

    QString pollConfigKey = QString(typeName + "/poll_file");
    if (Config::configuration()->value(pollConfigKey, "").toString().size() > 0) {
        usedDevicePollFilePath_ = Config::configuration()->value(pollConfigKey, "").toString();
    } else {
        usedDevicePollFilePath_ = devicePollFilePath_.arg(deviceNumber_);
    }

    if (deviceCount_ == 0) {
        sensordLogW() << "Cannot find any device for: " << typeName;
        setValid(false);
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

bool InputDevAdaptor::checkInputDevice(const QString& path, const QString& matchString, bool strictChecks)
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

bool InputDevAdaptor::openPollFile(QFile& pollFile, QIODevice::OpenMode mode) const
{
    pollFile.setFileName(usedDevicePollFilePath_);
    if (!(pollFile.exists() && pollFile.open(mode))) {
        sensordLogW() << "Unable to locate poll interval setting for " << deviceString_;
        return false;
    }
    return true;
}

unsigned int InputDevAdaptor::interval() const
{
    if (deviceNumber_ < 0) {
        return -1;
    }

    QFile pollFile;
    if(!openPollFile(pollFile, QIODevice::ReadOnly))
        return 0;

    char buf[16];
    if (pollFile.readLine(buf, sizeof(buf)) > 0) {
        return QString(buf).toUInt();
    }
    return 0;
}

bool InputDevAdaptor::setInterval(const unsigned int value, const int sessionId)
{
    Q_UNUSED(sessionId);

    sensordLogD() << "Setting poll interval for" << deviceString_ << " to " << value;

    QFile pollFile;
    if(!openPollFile(pollFile, QIODevice::WriteOnly))
        return false;

    QString frequencyString = QString("%1\n").arg(value);

    if (pollFile.write(frequencyString.toAscii().constData(), frequencyString.length()) < 0) {
        sensordLogW() << "Unable to set poll interval setting for " << deviceString_ << ":" << pollFile.error();
        return false;
    }
    return true;
}

void InputDevAdaptor::init()
{
    if (!getInputDevices(Config::configuration()->value(name() + "/input_match", name()).toString())) {
        sensordLogW() << "Input device not found.";
    }
    SysfsAdaptor::init();
}
