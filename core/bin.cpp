/**
   @file bin.cpp
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

#include "bin.h"
#include "pusher.h"
#include "consumer.h"
#include "filter.h"
#include "source.h"
#include "sink.h"
#include "callback.h"
#include "ringbuffer.h"
#include "logging.h"

Bin::Bin() :
    signalNewEvent_(this, &Bin::signalNewEvent)
{
}

Bin::~Bin()
{
}

void Bin::signalNewEvent()
{
    eventSignaled();
}

void Bin::eventSignaled()
{
    // filter new data
    Pusher* pusher;
    foreach (pusher, pushers_) {
        pusher->pushNewData();
    }
}

void Bin::start()
{
}

void Bin::stop()
{
}

void Bin::add(Pusher* pusher, const QString& name)
{
    Q_ASSERT(!pushers_.contains(name));
    Q_ASSERT(!filters_.contains(name));

    pusher->setReadyCallback(&signalNewEvent_);
    pushers_.insert(name, pusher);
}

void Bin::add(Consumer* consumer, const QString& name)
{
    Q_ASSERT(!consumers_.contains(name));
    Q_ASSERT(!filters_.contains(name));

    consumers_.insert(name, consumer);
}

void Bin::add(FilterBase* filter, const QString& name)
{
    Q_ASSERT(!pushers_.contains(name));
    Q_ASSERT(!consumers_.contains(name));
    Q_ASSERT(!filters_.contains(name));

    filters_.insert(name, filter);
}

bool Bin::join(const QString& producerName,
                  const QString& sourceName,
                  const QString& consumerName,
                  const QString& sinkName)
{
    bool joined = false;

    SourceBase* src = source(producerName, sourceName);
    SinkBase*   snk = sink(consumerName, sinkName);

    if (src && snk) {
        if (src->join(snk)) {

            joined = true;

        } else {
            sensordLogT() << " source "
                          << producerName << "/" << sourceName
                          << " and sink "
                          << consumerName << "/" << sinkName
                          << " are of incompatible types";
        }
    } else {
        if (!src) {
            sensordLogT() << "source "
                     << producerName << "/" << sourceName
                     << " not found";
        }
        if (!snk) {
            sensordLogT() << "sink "
                     << consumerName << "/" << sinkName
                     << " not found";
        }
    }

    return joined;
}

bool Bin::unjoin(const QString& producerName,
                  const QString& sourceName,
                  const QString& consumerName,
                  const QString& sinkName)
{
    SourceBase* src = source(producerName, sourceName);
    SinkBase*   snk = sink(consumerName, sinkName);

    bool unjoined = false;

    if (src && snk) {
        if (src->unjoin(snk)) {

            unjoined = true;

        } else {
            sensordLogT() << "Cannot unjoin sink & source. Possibly, they are not connected.";
        }
    } else {
        if (!src) {
            sensordLogT() << "source "
                     << producerName << "/" << sourceName
                     << " not found";
        }
        if (!snk) {
            sensordLogT() << "sink "
                     << consumerName << "/" << sinkName
                     << " not found";
        }
    }

    return unjoined;
}

SourceBase* Bin::source(const QString& producerName,
                        const QString& sourceName)
{
    SourceBase* source = 0;

    if (Producer* p = producer(producerName)) {
        source = p->source(sourceName);
    }

    return source;
}

SinkBase* Bin::sink(const QString& consumerName, const QString& sinkName)
{
    SinkBase* sink = 0;

    if (Consumer* c = consumer(consumerName)) {
        sink = c->sink(sinkName);
    }

    return sink;
}

Producer* Bin::producer(const QString& name)
{
    Producer* p = 0;

    if (pushers_.contains(name)) {
        p = pushers_.value(name);
    } else if (filters_.contains(name)) {
        p = filters_.value(name);
    }

    return p;
}

Consumer* Bin::consumer(const QString& name)
{
    Consumer* c = 0;

    if (consumers_.contains(name)) {
        c = consumers_.value(name);
    } else if (filters_.contains(name)) {
        c = filters_.value(name);
    }

    return c;
}
