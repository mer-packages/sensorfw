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

#include "tapadaptor.h"
#include "config.h"
#include "filters/utils.h"

#include <errno.h>

TapAdaptor::TapAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::SelectMode, Config::configuration()->value("tap_event_path").toString())
{
    tapBuffer_ = new DeviceAdaptorRingBuffer<TapData>(1024);
    addAdaptedSensor("tap", "Internal accelerometer tap events", tapBuffer_);
}

TapAdaptor::~TapAdaptor()
{
    delete tapBuffer_;
}

void TapAdaptor::processSample(int pathId, int fd)
{
    Q_UNUSED(pathId);

    read(fd, &ev_, sizeof(struct input_event));

    if (ev_.type == EV_MSC && ev_.code == MSC_GESTURE) {

        TapData* td = tapBuffer_->nextSlot();

        if (ev_.value == 0 || ev_.value == 1) {
            td->direction_ = TapData::X;
        }
        else if (ev_.value == 2 || ev_.value == 3) {
            td->direction_ = TapData::Y;
        }
        else if (ev_.value == 4 || ev_.value == 5) {
            td->direction_ = TapData::Z;
        }

        td->timestamp_ = Utils::getTimeStamp();

        // Every second is single, every second is double
        td->type_ = (ev_.value%2) ? TapData::DoubleTap : TapData::SingleTap;

        tapBuffer_->commit();
        tapBuffer_->wakeUpReaders();
    }
}
