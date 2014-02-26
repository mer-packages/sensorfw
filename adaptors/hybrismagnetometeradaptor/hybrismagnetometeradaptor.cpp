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

#include "config.h"
#include "hybrismagnetometeradaptor.h"
#include "logging.h"
#include "datatypes/utils.h"
#include <hardware/sensors.h>

HybrisMagnetometerAdaptor::HybrisMagnetometerAdaptor(const QString& id) :
    HybrisAdaptor(id,SENSOR_TYPE_MAGNETIC_FIELD)
{
    buffer = new DeviceAdaptorRingBuffer<TimedXyzData>(1);
    setAdaptedSensor("magnetometer", "Internal magnetometer coordinates", buffer);

    setDescription("Hybris magnetometer");
    //setStandbyOverride(false);
   // setDefaultInterval(50);
}

HybrisMagnetometerAdaptor::~HybrisMagnetometerAdaptor()
{
    delete buffer;
}

bool HybrisMagnetometerAdaptor::startSensor()
{
    if (!(HybrisAdaptor::startSensor()))
        return false;
    sensordLogD() << "HybrisMagnetometerAdaptor start\n";
    return true;
}

void HybrisMagnetometerAdaptor::stopSensor()
{
    HybrisAdaptor::stopSensor();
    sensordLogD() << "HybrisMagnetometerAdaptor stop\n";
}

void HybrisMagnetometerAdaptor::processSample(const sensors_event_t& data)
{
    TimedXyzData *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .0001);
    //uT to nT
    d->x_ = (data.acceleration.x * 1000);
    d->y_ = (data.acceleration.y * 1000);
    d->z_ = (data.acceleration.z * 1000);

    buffer->commit();
    buffer->wakeUpReaders();
}

void HybrisMagnetometerAdaptor::init()
{
}
