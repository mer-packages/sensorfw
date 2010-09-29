/**
   @file inputdevadaptor.cpp
   @brief Base class for input layer device adaptors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#define MAX_EVENT_DEV           16

InputDevAdaptor::InputDevAdaptor(const QString& id, int maxDeviceCount, int pollingInterval) :
    SysfsAdaptor(id),
    deviceCount_(0),
    maxDeviceCount_(maxDeviceCount),
    deviceNumber_(-1),
    pollingInterval_(pollingInterval),
    originalPollingInterval_(0)
{
    deviceSysPathString_ = Config::configuration()->value("device_sys_path").toString();
    devicePollFilePath_ = Config::configuration()->value("device_poll_file_path").toString();
}

InputDevAdaptor::~InputDevAdaptor()
{
}

bool InputDevAdaptor::startSensor(const QString& sensorId)
{
    if (!(SysfsAdaptor::startSensor(sensorId))) {
        return false;
    }

    // TODO: Skip these for devices that don't have poll interval handling.
    originalPollingInterval_ = getPollingInterval();
    if (originalPollingInterval_ < 0) {
        sensordLogW() << "Failed to read polling interval for " << deviceString_;
        originalPollingInterval_ = 0;
    }

    if (!setPollingInterval(pollingInterval_)) {
        sensordLogW() << "Failed to adjust polling interval for " << deviceString_;
    }
    return true;
}

void InputDevAdaptor::stopSensor(const QString& sensorId)
{
    SysfsAdaptor::stopSensor(sensorId);

    if (!setPollingInterval(originalPollingInterval_)) {
        sensordLogW() << "Failed to restore polling interval for " << deviceString_;
    }
}

int InputDevAdaptor::getInputDevices(QString matchString)
{
    int deviceNumber = 0;
    QString deviceName;
    deviceCount_ = 0;
    deviceString_ = matchString;

    // Check if this device name is defined in configuration
    QString configKey = QString("dev_%1").arg(matchString);
    deviceName = Config::configuration()->value(configKey, "DEV_NOT_FOUND").toString();

    // Do not perform strict checks for the input device
    if (deviceName != "DEV_NOT_FOUND" && checkInputDevice(deviceName, matchString, false)) {
        addPath(deviceName, deviceCount_);
        deviceCount_++;
        deviceNumber_ = deviceNumber;
        return deviceCount_;
     }

    // No configuration for this device, try find the device from the device system path
    while (deviceNumber < MAX_EVENT_DEV && deviceCount_ < maxDeviceCount_) {
        deviceName = deviceSysPathString_.arg(deviceNumber);
        if (checkInputDevice(deviceName, matchString)) {
            addPath(deviceName, deviceCount_);
            deviceCount_++;
            deviceNumber_ = deviceNumber;
        }
        deviceNumber++;
    }

    if (deviceCount_ == 0) {
        sensordLogW() << "Cannot find any device for: " << matchString;
        isValid_ = false;
    }

    return deviceCount_;
}

int InputDevAdaptor::getEvents(int fd)
{
    int bytes = read(fd, evlist_, sizeof(struct input_event)*64);

    if (bytes % sizeof(struct input_event)) {
        sensordLogW() << "Short read or stray bytes.";
    }

    return bytes/sizeof(struct input_event);
}

void InputDevAdaptor::processSample(int pathId, int fd)
{
    int numEvents = getEvents(fd);

    for (int i = 0; i < numEvents; i++) {
        switch (evlist_[i].type) {
            case EV_SYN:
                interpretSync(pathId);
                break;
            default:
                interpretEvent(pathId, &(evlist_[i]));
                break;
        }

    }
}

bool InputDevAdaptor::checkInputDevice(QString path, QString matchString, bool strictChecks)
{
    int fd;
    char deviceName[256] = {0,};
    bool check = true;

    fd = open(path.toLocal8Bit().constData(), O_RDONLY);
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
            sensordLogT() << "\"" << matchString << "\"" << "matched in device name: " << deviceName;
            check = true;
        } else {
            check = false;
        }
    }

    close(fd);

    return check;
}

int InputDevAdaptor::getPollingInterval()
{
    if (deviceNumber_ < 0) {
        return -1;
    }

    int result  = -1;

    // TODO: Clean this up somehow..
    // Check if this device name is defined in configuration
    QString configKey = QString("dev_poll_%1").arg(deviceString_);
    QFile pollFile;

    if (Config::configuration()->value(configKey, "").toString().size() > 0) {
        pollFile.setFileName(Config::configuration()->value(configKey, "").toString());
    } else {
        pollFile.setFileName(devicePollFilePath_.arg(deviceNumber_));
    }

    if (!(pollFile.exists() && pollFile.open(QIODevice::ReadOnly))) {
        sensordLogW() << "Unable to locate poll interval setting for " << deviceString_;
        return result;
    }

    char buf[16];
    if (pollFile.readLine(buf, sizeof(buf)) > 0) {
        result = QString(buf).toInt();
    }

    pollFile.close();
    return result;
}

bool InputDevAdaptor::setPollingInterval(int f)
{
    sensordLogD() << "Setting poll interval for " << deviceString_ << " to " << f;

    // TODO: Clean this up somehow..
    // Check if this device name is defined in configuration
    QString configKey = QString("dev_poll_%1").arg(deviceString_);
    QFile pollFile;

    if (Config::configuration()->value(configKey, "").toString().size() > 0) {
        pollFile.setFileName(Config::configuration()->value(configKey, "").toString());
    } else {
        pollFile.setFileName(devicePollFilePath_.arg(deviceNumber_));
    }

    if (!(pollFile.exists() && pollFile.open(QIODevice::WriteOnly))) {
        sensordLogW() << "Unable to locate poll interval setting for " << deviceString_;
        return false;
    }
    QString frequencyString = QString("%1\n").arg(f);

    if (pollFile.write(frequencyString.toAscii().constData(), frequencyString.length()) < 0) {
        sensordLogW() << "Unable to set poll interval setting for " << deviceString_ << ":" << pollFile.error();
    }

    pollFile.close();
    return true;
}
