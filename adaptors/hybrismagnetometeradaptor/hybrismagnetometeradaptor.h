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

#ifndef HYBRISMAGNETOMETERADPTOR_H
#define HYBRISMAGNETOMETERADPTOR_H
#include "hybrisadaptor.h"

#include <QString>
#include <QStringList>
#include <linux/input.h>
#include "deviceadaptorringbuffer.h"
#include "datatypes/orientationdata.h"
#include <QTime>

/**
 * @brief Adaptor for hybris magnetometer.
 *
 * Adaptor for internal magnetometer
 * Uses hybris sensor daemon driver interface in interval
 * polling mode, i.e. values are read with given constant interval.
 */
class HybrisMagnetometerAdaptor : public HybrisAdaptor
{
    Q_OBJECT

public:
    static DeviceAdaptor* factoryMethod(const QString& id) {
        return new HybrisMagnetometerAdaptor(id);
    }
    HybrisMagnetometerAdaptor(const QString& id);
    ~HybrisMagnetometerAdaptor();

    bool startSensor();
    void stopSensor();

protected:
    void processSample(const sensors_event_t& data);
    void init();

private:
    DeviceAdaptorRingBuffer<TimedXyzData>* buffer;
    int sensorType;

};
#endif
