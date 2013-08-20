/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com
**
**
** $QT_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "hybrisalsadaptor.h"
#include "logging.h"
#include "datatypes/utils.h"
#include <android/hardware/sensors.h>

HybrisAlsAdaptor::HybrisAlsAdaptor(const QString& id) :
    HybrisAdaptor(id,SENSOR_TYPE_LIGHT)
{
    buffer = new DeviceAdaptorRingBuffer<TimedUnsigned>(1);
    setAdaptedSensor("als", "Internal ambient light sensor lux values", buffer);
   // setDefaultInterval(50);
    setDescription("Hybris als");
}

HybrisAlsAdaptor::~HybrisAlsAdaptor()
{
    delete buffer;
}

bool HybrisAlsAdaptor::startSensor()
{
    if (!(HybrisAdaptor::startSensor()))
        return false;

    sensordLogD() << "Hybris HybrisAlsAdaptor start\n";
    return true;
}

void HybrisAlsAdaptor::stopSensor()
{
    HybrisAdaptor::stopSensor();
    sensordLogD() << "Hybris HybrisAlsAdaptor stop\n";
}

void HybrisAlsAdaptor::processSample(const sensors_event_t& data)
{
    TimedUnsigned *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .0001);
    d->value_ = data.light;

    buffer->commit();
    buffer->wakeUpReaders();
}

void HybrisAlsAdaptor::init()
{
}
