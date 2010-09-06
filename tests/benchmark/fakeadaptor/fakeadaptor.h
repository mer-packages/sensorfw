/**
   @file fakeadaptor.h
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

#ifndef FAKEADAPTOR_H
#define FAKEADAPTOR_H

#include "sensord/deviceadaptor.h"
#include "sensord/deviceadaptorringbuffer.h"
#include "datatypes/timedunsigned.h"
#include <QTime>
#include <QThread>

class FakeAdaptor;

class FakeAdaptorThread : public QThread
{
    Q_OBJECT;
public: 
    FakeAdaptorThread(FakeAdaptor *parent);
    void run();
    bool running;

private:
    FakeAdaptor *parent_;
};

/**
 * @brief Adaptor faking another adaptor input with generated data
 */
class FakeAdaptor : public DeviceAdaptor
{
    Q_OBJECT;
public:
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new FakeAdaptor(id);
    }

    bool startAdaptor();
    void stopAdaptor();

    bool startSensor(const QString& sensorId);
    void stopSensor(const QString& sensorId);

    void pushNewData(int& data);

    int interval_;
    
protected:
    FakeAdaptor(const QString& id);

private:
    FakeAdaptorThread* t;
    DeviceAdaptorRingBuffer<TimedUnsigned>* buffer_;
};

#endif

