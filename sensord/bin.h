/**
   @file bin.h
   @brief Bin

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

#ifndef BIN_H
#define BIN_H

#include "callback.h"
#include <QThread>
#include <QHash>
#include <QAtomicInt>
#include <QMutex>
#include <QWaitCondition>

class SourceBase;
class SinkBase;
class Pusher;
class Producer;
class Consumer;
class FilterBase;

class Bin;

// hide QThread so that its methods are not directly accessible via Bin
class PrivateThread : public QThread
{
public:
    PrivateThread(Bin& t) :
        thread_(t)
    {}

private:
    void run();

    Bin& thread_;
};

template <class TYPE>
class RingBuffer;

class Bin
{
friend class PrivateThread;

public slots:
    void start();
    void stop();
    void control(const QString& command);

public:
    class Command;

    Bin();
    ~Bin();

    // TODO: remove these
    void add(Pusher*     pusher,   const QString& name);
    void add(Consumer*   consumer, const QString& name);
    void add(FilterBase* filter,   const QString& name);

    // TODO: bad name; joining a thread means something else
    bool join(const QString& producerName,
              const QString& sourceName,
              const QString& consumerName,
              const QString& sinkName);

    bool unjoin(const QString& producerName,
              const QString& sourceName,
              const QString& consumerName,
              const QString& sinkName);

protected:
    SourceBase* source(const QString& producerName, const QString& sourceName);
    SinkBase*   sink(const QString& consumerName, const QString& sinkName);
    Producer*   producer(const QString& name);
    Consumer*   consumer(const QString& name);

private:
    class CommandReader;

    friend class Command;
    friend class StopCommand; // TODO: remove

    PrivateThread               thread_;
    RingBuffer<Command*>*       commands_;
    CommandReader*              commandReader_;
    QHash<QString, Pusher*>     pushers_;
    QHash<QString, Consumer*>   consumers_;
    QHash<QString, FilterBase*> filters_;
    Callback<Bin>               signalNewEvent_;
    QAtomicInt                  isNewEvent_;
    QMutex                      mutex_;
    QWaitCondition              newEvent_;
    bool                        running_;

    void run();
    void signalNewEvent();
    void waitForNewEvents();
    void handleEvents();
};

typedef Bin* (*ProcessingBinFactoryMethod)(const QString& id);

#endif
