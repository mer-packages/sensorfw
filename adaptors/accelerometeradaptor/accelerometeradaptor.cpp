/**
   @file accelerometeradaptor.cpp
   @brief Contains AccelerometerAdaptor.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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


#include "accelerometeradaptor.h"

#include "logging.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <QMap>

#include "datatypes/utils.h"

#define DEVICE_MATCH_STRING "accelerometer"

AccelerometerAdaptor::AccelerometerAdaptor(const QString& id) :
    InputDevAdaptor(id, 1)
{
    //This was previously in the base class, but it's not
    //possible call virtual methods from base class constructor.
    //TODO: find a way to get rid of all child classes calling this
    //manually.
    if (!getInputDevices(DEVICE_MATCH_STRING)) {
        sensordLogW() << "Input device not found.";
    }

    accelerometerBuffer_ = new DeviceAdaptorRingBuffer<OrientationData>(128);
    addAdaptedSensor("accelerometer", "Internal accelerometer coordinates", accelerometerBuffer_);

    // Set Metadata
    setDescription("Input device accelerometer adaptor (lis302d)");
    introduceAvailableDataRange(DataRange(-2048, 2048, 1));
    introduceAvailableInterval(DataRange(0, 0, 0));
    introduceAvailableInterval(DataRange(10, 1000, 0)); // -> [1,100] Hz
    setDefaultInterval(0);
}

AccelerometerAdaptor::~AccelerometerAdaptor()
{
    delete accelerometerBuffer_;
}

void AccelerometerAdaptor::interpretEvent(int src, struct input_event *ev)
{
    Q_UNUSED(src);

    switch (ev->type) {
        case EV_ABS:
            switch (ev->code) {
                case ABS_X:
                    orientationValue_.x_ = ev->value;
                    break;
                case ABS_Y:
                    orientationValue_.y_ = ev->value;
                    break;
                case ABS_Z:
                    orientationValue_.z_ = ev->value;
                    break;
            }
            break;

    }
}

void AccelerometerAdaptor::interpretSync(int src)
{
    Q_UNUSED(src);
    commitOutput();
}

void AccelerometerAdaptor::commitOutput()
{
    OrientationData* d = accelerometerBuffer_->nextSlot();

    d->timestamp_ = Utils::getTimeStamp();
    d->x_ = -orientationValue_.y_;
    d->y_ = orientationValue_.x_;
    d->z_ = orientationValue_.z_;

    accelerometerBuffer_->commit();
    accelerometerBuffer_->wakeUpReaders();
}

unsigned int AccelerometerAdaptor::evaluateIntervalRequests(int& sessionId) const
{
    unsigned int highestValue = 0;
    int winningSessionId = -1;

    if (m_intervalMap.size() == 0)
    {
        sessionId = winningSessionId;
        return defaultInterval();
    }

    // Get the smallest positive request, 0 is reserved for HW wakeup
    QMap<int, unsigned int>::const_iterator it;
    it = m_intervalMap.begin();
    highestValue = it.value();
    winningSessionId = it.key();

    for (++it; it != m_intervalMap.end(); ++it)
    {
        if ((it.value() < highestValue) && (it.value() > 0)) {
            highestValue = it.value();
            winningSessionId = it.key();
        }
    }

    sessionId = winningSessionId;
    return highestValue > 0 ? highestValue : defaultInterval();
}
