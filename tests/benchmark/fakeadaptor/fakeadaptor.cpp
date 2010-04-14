/**
   @file fakeadaptor.cpp
   @brief Fake adaptor for synthesizing input

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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

#include <QtDebug>
#include <QFile>
#include "fakeadaptor.h"
#include <errno.h>
#include "filters/utils.h"

FakeAdaptor::FakeAdaptor(const QString& id) : DeviceAdaptor(id), interval_(1)
{
    t = new FakeAdaptorThread(this);

    buffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(1024);
    addAdaptedSensor("als", "Internal ambient light sensor lux values", buffer_);
}

bool FakeAdaptor::startAdaptor()
{
    QFile file("/tmp/sensorTestSampleRate");

    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to get rate from" << file.fileName() << "- using 1000Hz (open)";
        return true;
    }

    interval_ = atoi(file.readLine().data());
    if (interval_ <= 0) {
        qDebug() << "Failed to get rate from" << file.fileName() << "- using 1000Hz (readline)";
        return true;
    }

    file.close();
    return true;
}

void FakeAdaptor::stopAdaptor()
{
}

bool FakeAdaptor::startSensor(const QString& sensorId)
{
    Q_UNUSED(sensorId);

    qDebug() << "Pushing fake ALS data with" << interval_ << " msec interval";
    // Start pushing data
    t->running = true;
    t->start();
    return true;
}

void FakeAdaptor::stopSensor(const QString& sensorId)
{
    Q_UNUSED(sensorId);

    // Stop pushing data
    t->running = false;
    t->wait();
    qDebug() << "sensor stopped";
}


void FakeAdaptor::pushNewData(int& data)
{
    TimedUnsigned* lux = buffer_->nextSlot();

    lux->timestamp_ = Utils::getTimeStamp();
    lux->value_ = data;

    buffer_->commit();
    buffer_->wakeUpReaders();
}

FakeAdaptorThread::FakeAdaptorThread(FakeAdaptor *parent) : running(false), parent_(parent)
{
    qDebug() << "Data pusher for ALS";
}

void FakeAdaptorThread::run()
{
    int i = 0;
    while(running) {
        QThread::msleep(parent_->interval_);
        parent_->pushNewData(i);
        i++;
    }
}
