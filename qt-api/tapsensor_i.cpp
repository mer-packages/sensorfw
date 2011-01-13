/**
   @file tapsensor_i.cpp
   @brief Interface for TapSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
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

QDBusAbstractInterface* TapSensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    // ToDo: see which arguments can be made explicit
    return new TapSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}

TapSensorChannelInterface::TapSensorChannelInterface(const QString& path, int sessionId)
    : AbstractSensorChannelInterface(path, TapSensorChannelInterface::staticInterfaceName, sessionId)
{
    type_ = SingleDouble;
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(output()));
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

void TapSensorChannelInterface::dataReceived()
{
    QVector<TapData> values;
    while (read<TapData>(values)) {
        foreach(TapData value, values) {
            if (type_ == Single) {
                emit dataAvailable(Tap(value));
            } else if (timer->isActive()) {
                if ((!tapValues_.isEmpty()) && (tapValues_.first().direction_ == value.direction_)) {
                    timer->stop();
                    tapValues_.removeFirst();
                    value.type_ = TapData:: DoubleTap;
                    tapValues_.prepend(value);
                    output();
                } else {
                    output();
                    tapValues_.prepend(value);
                    timer->start(doubleClickInteval);
                }
            } else {
                tapValues_.prepend(value);
                timer->start(doubleClickInteval);
            }
        }
        values.clear();
    }
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
