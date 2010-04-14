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

#include "filters/utils.h"

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

    accelerometerBuffer_ = new DeviceAdaptorRingBuffer<OrientationData>(1024);
    addAdaptedSensor("accelerometer", "Internal accelerometer coordinates", accelerometerBuffer_);
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
    d->x_ = orientationValue_.x_;
    d->y_ = orientationValue_.y_;
    d->z_ = orientationValue_.z_;
    
    accelerometerBuffer_->commit();
    accelerometerBuffer_->wakeUpReaders();
}
