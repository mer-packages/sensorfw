/**
   @file tapadaptor.cpp
   @brief TapAdaptor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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
#include <QTime>
#include <QTimer>

#include "tapadaptor.h"
#include "config.h"
#include "filters/utils.h"

#include <errno.h>

#define DEVICE_MATCH_STRING "accelerometer"
#define DOUBLECLICK_INTERVAL 500

TapAdaptor::TapAdaptor(const QString& id) :
    InputDevAdaptor(id, 1),
    waitingForDouble(false)
{
    //This was previously in the base class, but it's not
    //possible call virtual methods from base class constructor.
    //TODO: find a way to get rid of all child classes calling this
    //manually.
    if (!getInputDevices(DEVICE_MATCH_STRING)) {
        sensordLogW() << "Input device not found.";
    }

    tapBuffer_ = new DeviceAdaptorRingBuffer<TapData>(1024);
    addAdaptedSensor("tap", "Internal accelerometer tap events", tapBuffer_);
}

TapAdaptor::~TapAdaptor()
{
    delete tapBuffer_;
}

void TapAdaptor::interpretEvent(int src, struct input_event *ev)
{
    Q_UNUSED(src);

    TapData::Direction dir;

    if (ev->type == EV_KEY && ev->value == 1) {
        switch (ev->code) {
            case BTN_X:
                dir = TapData::X;
                break;
            case BTN_Y:
                dir = TapData::Y;
                break;
            case BTN_Z:
                dir = TapData::Z;
                break;
        }
        if ( (dir == tapValue_.direction_) &&
                (waitingForDouble) )
            tapValue_.type_ = TapData::DoubleTap;
        else tapValue_.type_ = TapData::SingleTap;

        tapValue_.direction_ = dir;
    }
}

void TapAdaptor::interpretSync(int src)
{
    Q_UNUSED(src);
    if (!waitingForDouble) {
        timerId = startTimer(DOUBLECLICK_INTERVAL);
        waitingForDouble = true;
    }
}

void TapAdaptor::timerEvent(QTimerEvent* event)
{
    waitingForDouble = false;
    commitOutput();
    killTimer(timerId);
}

void TapAdaptor::commitOutput()
{
    TapData* d = tapBuffer_->nextSlot();

    d->timestamp_ = Utils::getTimeStamp();
    d->direction_ = tapValue_.direction_;
    d->type_ = tapValue_.type_;

    tapBuffer_->commit();
    tapBuffer_->wakeUpReaders();
}
