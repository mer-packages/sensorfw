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
#include <QFile>
#include <linux/input.h>

/**
 * @brief Base class for adaptors accessing device drivers through
 * Linux Input Device subsytem.
 */
class InputDevAdaptor : public SysfsAdaptor
{
public:
    /**
     * Constructor.
     *
     * @param id The id for the adaptor.
     * @param maxDeviceCount Maximum number of devices that can be
     *                       associated with this adaptor.
     */
    InputDevAdaptor(const QString& id, int maxDeviceCount = 1);

    /**
     * Destructor.
     */
    virtual ~InputDevAdaptor();

    /**
     * Number of available input devices.
     *
     * @return number of available input devices.
     */
    int getDeviceCount() const;

protected:
    /**
     * Verify whether the input device handle on given path is of a certain
     * device type.
     *
     * @param path path of the device file.
     * @param matchString string to match in the device name acquired from
     *                    the input device system.
     * @param strictChecks whether to perform more extensive checks for
     *                     the input device.
     * @return is given device valid or not.
     */
    virtual bool checkInputDevice(const QString& path, const QString& matchString, bool strictChecks = true) const;

    /**
     * Interpret a read event and store the received value.
     *
     * @param src Event source.
     * @param ev  Read event.
     */
    virtual void interpretEvent(int src, struct input_event *ev) = 0;

    /**
     * Interpret a a synchronization event from the device.
     *
     * @param src Event source.
     * @param ev  Read event.
     */
    virtual void interpretSync(int src, struct input_event *ev) = 0;

    /**
     * Scans through the /dev/input/event* device handles and registers the
     * ones that pass the test with the #checkInputDevice method.
     *
     * @param matchString String to match in device name fields.
     * @return Number of devices detected.
     */
    int getInputDevices(const QString& matchString);

    void processSample(int pathId, int fd);

    virtual unsigned int interval() const;

    virtual bool setInterval(const unsigned int value, const int sessionId);

private:
    /**
     * Read events from file descriptor. The read events are stored in
     * #evlist_ array.
     *
     * @param fd File descriptor to read from.
     * @return Number of read events.
     */
    int getEvents(int fd);

    /**
     * Opens a file with given mode flags.
     *
     * @param pollFile file path.
     * @param mode mode flags.
     * @return was file opened succesfully.
     */
    bool openPollFile(QFile& pollFile, QIODevice::OpenMode mode) const;

    QString usedDevicePollFilePath_; /**< sysfs path to input device poll file */
    QString deviceString_;           /**< input device name */
    int deviceCount_;                /**< number of available input devices */
    const int maxDeviceCount_;       /**< maximum number of supported devices */
    input_event evlist_[64];         /**< input event buffer */
};

#endif
