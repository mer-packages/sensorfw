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

#include "hybrisaccelerometeradaptor.h"
#include "logging.h"
#include "datatypes/utils.h"
#include <hardware/sensors.h>

#define GRAVITY_RECIPROCAL_THOUSANDS 101.971621298

HybrisAccelerometerAdaptor::HybrisAccelerometerAdaptor(const QString& id) :
    HybrisAdaptor(id,SENSOR_TYPE_ACCELEROMETER)
{
    buffer = new DeviceAdaptorRingBuffer<AccelerationData>(1);
    setAdaptedSensor("accelerometer", "Internal accelerometer coordinates", buffer);

    setDescription("Hybris accelerometer");
//    setDefaultInterval(50);
}

HybrisAccelerometerAdaptor::~HybrisAccelerometerAdaptor()
{
    delete buffer;
}

bool HybrisAccelerometerAdaptor::startSensor()
{
    if (!(HybrisAdaptor::startSensor()))
        return false;

    sensordLogD() << "Hybris AccelAdaptor start\n";
    return true;
}

void HybrisAccelerometerAdaptor::stopSensor()
{
    HybrisAdaptor::stopSensor();
    sensordLogD() << "Hybris AccelAdaptor stop\n";
}

void HybrisAccelerometerAdaptor::processSample(const sensors_event_t& data)
{
    AccelerationData *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .001);
    // sensorfw wants milli-G'

    d->x_ = data.acceleration.x * GRAVITY_RECIPROCAL_THOUSANDS;
    d->y_ = data.acceleration.y * GRAVITY_RECIPROCAL_THOUSANDS;
    d->z_ = data.acceleration.z * GRAVITY_RECIPROCAL_THOUSANDS;

    buffer->commit();
    buffer->wakeUpReaders();
}

//void HybrisAccelerometerAdaptor::init()
//{
////    introduceAvailableDataRange(DataRange(-HybrisAdaptor::maxRange, HybrisAdaptor::maxRange , 1));
////    introduceAvailableInterval(DataRange(10, 586, 0));
//}
