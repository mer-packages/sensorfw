/**
   @file inputdevadaptor.h
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

#ifndef INPUTDEVADAPTOR_H
#define INPUTDEVADAPTOR_H

#include "sysfsadaptor.h"
#include <QString>
#include <QStringList>
#include <linux/input.h>
#include "filterproperty.h"

/**
 * @brief Base class for adaptors accessing device drivers through Linux Input
 * Device subsytem.
 *
 */
class InputDevAdaptor : public SysfsAdaptor {
    Q_OBJECT;

public:
    /**
     * Constructor.
     * @param id The id for the adaptor.
     * @param maxDeviceCount Maximum number of devices that can be associated with this adaptor
     */
    InputDevAdaptor(const QString& id, int maxDeviceCount = 1);
    virtual ~InputDevAdaptor();

    virtual void init();

    int getDeviceCount() { return deviceCount_; }

protected:
    /**
     * Verify whether the input device handle on given path is of a certain device type.
     * @param path path of the device file.
     * @param matchString stringt to match in the device name acquired from the
     * input device system.
     * @param strictChecks whether to perform more extensive checks for the input device.
     * @return True if yes, false if not.
     */
    virtual bool checkInputDevice(const QString& path, const QString& matchString, bool strictChecks = true);

    /**
     * Interpret a read event and store the received value.
     * @param src Event source.
     * @param ev  Read event.
     */
    virtual void interpretEvent(int src, struct input_event *ev) = 0;

    /**
     * Interpret a a synchronization event from the device.
     */
    virtual void interpretSync(int src, struct input_event *ev) = 0;

    /**
     * Scans through the /dev/input/event* device handles and registers the
     * ones that pass the test with the #checkInputDevice method.
     * @param typeName device type name
     *
     * @return Number of devices detected.
     */
    int getInputDevices(const QString& typeName);

    /**
     * Read and process data. Run when sysfsadaptor has detected new available
     * data.
     * @param pathId PathId for the file that had event. Id for first found event
     *               handle is 0, for second it's 1, etc.
     * @param fd     Open file descriptor with new data. See #SysfsAdaptor::processSample()
     */
    void processSample(int pathId, int fd);

    virtual unsigned int interval() const;
    virtual bool setInterval(const unsigned int value, const int sessionId);

private:

    /**
     * Read events from file descriptor. The read events are stored in #evlist
     * array.
     * @param fd File descriptor to read from.
     * @return Number of read events.
     */
    int getEvents(int fd);

    QString deviceSysPathString_;
    QString devicePollFilePath_;
    QString usedDevicePollFilePath_;
    QString deviceString_;
    int deviceCount_;
    int maxDeviceCount_;
    int deviceNumber_;
    unsigned int originalPollingInterval_;
    input_event evlist_[64];
};

#endif
