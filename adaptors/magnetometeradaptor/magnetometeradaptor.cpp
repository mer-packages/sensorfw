/**
   @file magnetometeradaptor.cpp
   @brief MagnetometerAdaptor

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

#include "logging.h"
#include "magnetometeradaptor.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include "filters/utils.h"
#include <QFile>

#define DEVICE_MATCH_STRING "magnetometer"

MagnetometerAdaptor::MagnetometerAdaptor(const QString& id) :
    InputDevAdaptor(id, 1),
    originalPollingRate_(0)
{
    //This was previously in the base class, but it's not
    //possible call virtual methods from base class constructor.
    //TODO: find a way to get rid of all child classes calling this
    //manually.
    if (!getInputDevices(DEVICE_MATCH_STRING)) {
        sensordLogW() << "Input device not found.";
    }

    magnetometerBuffer_ = new DeviceAdaptorRingBuffer<MagnetometerData>(1024);
    addAdaptedSensor("magnetometer", "Internal magnetometer coordinates", magnetometerBuffer_);

    introduceAvailableDataRange(DataRange(-4096, 4096, 1));
}

MagnetometerAdaptor::~MagnetometerAdaptor()
{
    delete magnetometerBuffer_;
}

void MagnetometerAdaptor::interpretEvent(int src, struct input_event *ev)
{
    Q_UNUSED(src);

    switch (ev->type) {
        case EV_ABS:
            switch (ev->code) {
                case ABS_X:
                    magnetometerValue_.x_ = ev->value;
                    break;
                case ABS_Y:
                    magnetometerValue_.y_ = ev->value;
                    break;
                case ABS_Z:
                    magnetometerValue_.z_ = ev->value;
                    break;
            }
            break;

    }
}

void MagnetometerAdaptor::interpretSync(int src)
{
    Q_UNUSED(src);
    commitOutput();
}

void MagnetometerAdaptor::commitOutput()
{
    OrientationData* d = magnetometerBuffer_->nextSlot();

    d->timestamp_ = Utils::getTimeStamp();
    d->x_ = magnetometerValue_.x_;
    d->y_ = magnetometerValue_.y_;
    d->z_ = magnetometerValue_.z_;

    magnetometerBuffer_->commit();
    magnetometerBuffer_->wakeUpReaders();
}

