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
    buffer = new DeviceAdaptorRingBuffer<CalibratedMagneticFieldData>(1);
    setAdaptedSensor("magnetometer", "Internal magnetometer coordinates", buffer);

    setDescription("Hybris magnetometer");
    //setStandbyOverride(false);
    setDefaultInterval(50);
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
    CalibratedMagneticFieldData *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .001);
    //uT
    d->x_ = (data.magnetic.x * 1000);
    d->y_ = (data.magnetic.y * 1000);
    d->z_ = (data.magnetic.z * 1000);
    d->level_= data.magnetic.status;
#ifdef SENSORS_DEVICE_API_VERSION_1_1
    d->rx_ = data.uncalibrated_magnetic.x_uncalib * 1000;
    d->ry_ = data.uncalibrated_magnetic.y_uncalib * 1000;
    d->rz_ = data.uncalibrated_magnetic.z_uncalib * 1000;
    qDebug() << Q_FUNC_INFO
             << "uncalibrated"
             << data.uncalibrated_magnetic.x_uncalib * 1000
             << data.uncalibrated_magnetic.y_uncalib * 1000
             << data.uncalibrated_magnetic.z_uncalib * 1000;
    qDebug() << Q_FUNC_INFO
             << "bias"
             << data.uncalibrated_magnetic.x_bias * 1000
             << data.uncalibrated_magnetic.y_bias * 1000
             << data.uncalibrated_magnetic.z_bias * 1000;
#else
    d->rx_ = data.magnetic.x * 1000;
    d->ry_ = data.magnetic.y * 1000;
    d->rz_ = data.magnetic.z * 1000;

#endif
    qDebug() << Q_FUNC_INFO << d->x_ << d->y_ << d->z_ << data.magnetic.status;
    buffer->commit();
    buffer->wakeUpReaders();
}

void HybrisMagnetometerAdaptor::init()
{
}
