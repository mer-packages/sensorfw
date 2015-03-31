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

#include "hybrisgyroscopeadaptor.h"
#include "logging.h"
#include "datatypes/utils.h"
#include <hardware/sensors.h>
#include <math.h>

#define RADIANS_TO_DEGREESECONDS 57295.7795
#define RADIANS_TO_DEGREES 57.2957795

HybrisGyroscopeAdaptor::HybrisGyroscopeAdaptor(const QString& id) :
    HybrisAdaptor(id,SENSOR_TYPE_GYROSCOPE)
{
    buffer = new DeviceAdaptorRingBuffer<TimedXyzData>(1);
    setAdaptedSensor("gyroscopeadaptor", "Internal gyroscope coordinates", buffer);

    setDescription("Hybris gyroscope");
    setDefaultInterval(50);
}

HybrisGyroscopeAdaptor::~HybrisGyroscopeAdaptor()
{
    delete buffer;
}

bool HybrisGyroscopeAdaptor::startSensor()
{
    if (!(HybrisAdaptor::startSensor()))
        return false;

    sensordLogD() << "HybrisGyroscopeAdaptor start\n";
    return true;
}

void HybrisGyroscopeAdaptor::stopSensor()
{
    HybrisAdaptor::stopSensor();
    sensordLogD() << "HybrisGyroscopeAdaptor stop\n";
}


void HybrisGyroscopeAdaptor::processSample(const sensors_event_t& data)
{

    TimedXyzData *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .001);
    d->x_ = (data.gyro.x) * RADIANS_TO_DEGREES * 1000;
    d->y_ = (data.gyro.y) * RADIANS_TO_DEGREES * 1000;
    d->z_ = (data.gyro.z) * RADIANS_TO_DEGREES * 1000;
    buffer->commit();
    buffer->wakeUpReaders();
}


void HybrisGyroscopeAdaptor::init()
{

}
