/**
   @file alsevdevadaptor.cpp
   @brief Plugin for ALSAdaptorEvdev

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2015 Jolla

   @author Lorn Potter <lorn.potter@jolla.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Markus Lehtonen <markus.lehtonen@nokia.com>

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

#include "alsevdevadaptor.h"
#include "config.h"
#include "logging.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <QMap>

#include "datatypes/utils.h"

ALSAdaptorEvdev::ALSAdaptorEvdev(const QString& id) :
    InputDevAdaptor(id, 1)
{
    alsBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(1);
    setAdaptedSensor("als", "Internal ambient light sensor lux values", alsBuffer_);
    setDescription("Input device als adaptor");
    powerStatePath_ = Config::configuration()->value("als/powerstate_path").toByteArray();
    introduceAvailableDataRange(DataRange(0, 4095, 1));
    setDefaultInterval(10);
}

ALSAdaptorEvdev::~ALSAdaptorEvdev()
{
}

void ALSAdaptorEvdev::interpretEvent(int src, struct input_event *ev)
{
    Q_UNUSED(src);

    switch (ev->type) {
    case EV_ABS:
        switch (ev->code) {
        case ABS_X:
        case ABS_MISC:
            alsValue_ = ev->value;
            break;
        }
        break;
    }
}

void ALSAdaptorEvdev::interpretSync(int src, struct input_event *ev)
{
    Q_UNUSED(src);
    commitOutput(ev);
}

void ALSAdaptorEvdev::commitOutput(struct input_event *ev)
{
    TimedUnsigned* lux = alsBuffer_->nextSlot();
    lux->value_ = alsValue_;

    lux->timestamp_ = Utils::getTimeStamp(&(ev->time));

    alsBuffer_->commit();
    alsBuffer_->wakeUpReaders();
}

unsigned int ALSAdaptorEvdev::evaluateIntervalRequests(int& sessionId) const
{
    unsigned int highestValue = 0;
    int winningSessionId = -1;

    if (m_intervalMap.size() == 0) {
        sessionId = winningSessionId;
        return defaultInterval();
    }

    // Get the smallest positive request, 0 is reserved for HW wakeup
    QMap<int, unsigned int>::const_iterator it;
    it = m_intervalMap.begin();
    highestValue = it.value();
    winningSessionId = it.key();

    for (++it; it != m_intervalMap.constEnd(); ++it) {
        if ((it.value() < highestValue) && (it.value() > 0)) {
            highestValue = it.value();
            winningSessionId = it.key();
        }
    }

    sessionId = winningSessionId;
    return highestValue > 0 ? highestValue : defaultInterval();
}

bool ALSAdaptorEvdev::startSensor()
{
    if (!powerStatePath_.isEmpty()) {
        writeToFile(powerStatePath_, "1");
    }
    if (SysfsAdaptor::startSensor()) {
        return true;
    }
    return false;
}

void ALSAdaptorEvdev::stopSensor()
{
    if (!powerStatePath_.isEmpty()) {
        writeToFile(powerStatePath_, "0");
    }

    SysfsAdaptor::stopSensor();
}

bool ALSAdaptorEvdev::standby()
{
    stopSensor();
    return true;
}

bool ALSAdaptorEvdev::resume()
{
    startSensor();
    return true;
}
