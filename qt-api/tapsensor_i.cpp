/**
   @file tapsensor_i.cpp
   @brief Interface for TapSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
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

#include "sensormanagerinterface.h"
#include "tapsensor_i.h"

const char* TapSensorChannelInterface::staticInterfaceName = "local.TapSensor";

AbstractSensorChannelInterface* TapSensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    return new TapSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}

TapSensorChannelInterface::TapSensorChannelInterface(const QString& path, int sessionId)
    : AbstractSensorChannelInterface(path, TapSensorChannelInterface::staticInterfaceName, sessionId)
{
    type_ = SingleDouble;
    timer_ = new QTimer(this);
    timer_->setSingleShot(true);
    connect(timer_, SIGNAL(timeout()), this, SLOT(output()));
}

const TapSensorChannelInterface* TapSensorChannelInterface::listenInterface(const QString& id)
{
    return dynamic_cast<const TapSensorChannelInterface*>(interface(id));
}

TapSensorChannelInterface* TapSensorChannelInterface::controlInterface(const QString& id)
{
    return interface(id);
}

TapSensorChannelInterface* TapSensorChannelInterface::interface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, TapSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }
    return dynamic_cast<TapSensorChannelInterface*>(sm.interface(id));
}

bool TapSensorChannelInterface::dataReceivedImpl()
{
    QVector<TapData> values;
    if(!read<TapData>(values))
        return false;
    foreach(TapData value, values) {
        if (type_ == Single) {
            emit dataAvailable(Tap(value));
        } else if (timer_->isActive()) {
            if ((!tapValues_.isEmpty()) && (tapValues_.first().direction_ == value.direction_)) {
                timer_->stop();
                tapValues_.removeFirst();
                value.type_ = TapData::DoubleTap;
                tapValues_.prepend(value);
                output();
            } else {
                output();
                tapValues_.prepend(value);
                timer_->start(doubleClickInteval);
            }
        } else {
            tapValues_.prepend(value);
            timer_->start(doubleClickInteval);
        }
    }
    return true;
}

void TapSensorChannelInterface::setTapType(TapSelection type)
{
    tapValues_.clear();
    type_ = type;
}

TapSensorChannelInterface::TapSelection TapSensorChannelInterface::getTapType()
{
    return type_;
}

void TapSensorChannelInterface::output()
{
    if (type_ == Double)
    {
        if ((tapValues_.last().type_ == TapData::SingleTap) && (tapValues_.size() == 1))
        {
           tapValues_.removeLast();
           return;
        }

        if (tapValues_.last().type_ == TapData::DoubleTap)
            emit dataAvailable(Tap(tapValues_.takeLast()));
    }

    if (type_ == SingleDouble)
        emit dataAvailable(Tap(tapValues_.takeLast()));
}
