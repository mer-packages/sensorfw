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

#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "hybrisproximityadaptor.h"
#include "logging.h"
#include "datatypes/utils.h"
#include <android/hardware/sensors.h>
#include <fcntl.h>

HybrisProximityAdaptor::HybrisProximityAdaptor(const QString& id) :
    HybrisAdaptor(id,SENSOR_TYPE_PROXIMITY)
{
    buffer = new DeviceAdaptorRingBuffer<ProximityData>(1);
    setAdaptedSensor("proximity", "Internal proximity coordinates", buffer);

    setDescription("Hybris proximity");
}

HybrisProximityAdaptor::~HybrisProximityAdaptor()
{
    delete buffer;
}

bool HybrisProximityAdaptor::startSensor()
{
    QTimer::singleShot(500,this,SLOT(sendInitialData()));
    if (!(HybrisAdaptor::startSensor()))
        return false;
    sensordLogD() << "HybrisProximityAdaptor start\n";
    return true;
}

void HybrisProximityAdaptor::sendInitialData()
{
    QFile file("/proc/bus/input/devices");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        bool ok = false;
        QString inputDev;

        QTextStream in(&file);
        QString line = in.readLine();
        while (!line.isNull()) {
            if (ok && line.startsWith("S: Sysfs=")) {
                inputDev = line.split("=").at(1).section("/",-1);
                ok = false;
                break;
            }
            if (line.contains("proximity")) {
                ok = true;
            }
            line = in.readLine();
        }

        struct input_absinfo absinfo;
        int fd;
        inputDev.replace("input","event");
        inputDev.prepend("/dev/input/");

        if ((fd = open(inputDev.toLatin1(), O_RDONLY)) > 0) {

            if (!ioctl(fd, EVIOCGABS(ABS_DISTANCE), &absinfo)) {
                bool near = false;
                if (absinfo.value == 0)
                    near = true;

                ProximityData *d = buffer->nextSlot();

                d->timestamp_ = Utils::getTimeStamp();
                d->withinProximity_ = near;
                d->value_ = near ? 10 : 0;
                buffer->commit();
                buffer->wakeUpReaders();
            } else {
                qDebug() << "ioctl not opened" ;
            }
            close(fd);
        } else {
            qDebug() << "could not open proximity evdev";
        }
    }
}

void HybrisProximityAdaptor::stopSensor()
{
    HybrisAdaptor::stopSensor();
    sensordLogD() << "HybrisProximityAdaptor stop\n";
}

void HybrisProximityAdaptor::processSample(const sensors_event_t& data)
{
    ProximityData *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .0001);
    bool near = false;

    if (data.distance < maxRange) {
        near = true;
    }
    d->withinProximity_ = near;
    d->value_ = data.distance;

    buffer->commit();
    buffer->wakeUpReaders();
}

void HybrisProximityAdaptor::init()
{
}
