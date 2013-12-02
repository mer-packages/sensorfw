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

#include "hybrisorientationadaptor.h"
#include "logging.h"
#include "datatypes/utils.h"
#include <android/hardware/sensors.h>

/*
 * azimuth: angle between the magnetic north direction and the Y axis, around
 * the Z axis (0<=azimuth<360).
 * 0=North, 90=East, 180=South, 270=West
 *
 * pitch: Rotation around X axis (-180<=pitch<=180), with positive values when
 * the z-axis moves toward the y-axis.
 *
 * roll: Rotation around Y axis (-90<=roll<=90), with positive values when
 * the x-axis moves towards the z-axis.
 *
 **/

/*
 * not to be confused with orientation
 **/

HybrisOrientationAdaptor::HybrisOrientationAdaptor(const QString& id) :
    HybrisAdaptor(id,SENSOR_TYPE_ORIENTATION)
{
    buffer = new DeviceAdaptorRingBuffer<TimedXyzData>(1);
    setAdaptedSensor("orientation", "Internal orientation coordinates", buffer);

    setDescription("Hybris orientation");
//    setDefaultInterval(50);
}

HybrisOrientationAdaptor::~HybrisOrientationAdaptor()
{
    delete buffer;
}

bool HybrisOrientationAdaptor::startSensor()
{
    if (!(HybrisAdaptor::startSensor()))
        return false;

    sensordLogD() << "Hybris OrientationAdaptor start\n";
    return true;
}

void HybrisOrientationAdaptor::stopSensor()
{
    HybrisAdaptor::stopSensor();
    sensordLogD() << "Hybris OrientationAdaptor stop\n";
}

void HybrisOrientationAdaptor::processSample(const sensors_event_t& data)
{
    TimedXyzData *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .001);
    // sensorfw wants milli-G'
    d->x_ = data.data[0] * 1000; //azimuth
    d->y_ = data.data[1] * 1000; //pitch
    d->z_ = data.data[2] * 1000; //roll

//  qt's sensorfw plugin expects G == 9.81286, but it should be
    //9.80665
    buffer->commit();
    buffer->wakeUpReaders();
}

void HybrisOrientationAdaptor::init()
{
}
