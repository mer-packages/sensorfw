/**
   @file bin.h
   @brief Bin

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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
#include <QHash>

class SourceBase;
class SinkBase;
class Pusher;
class Producer;
class Consumer;
class FilterBase;

template <class TYPE>
class RingBuffer;

/**
 * Bin is a container for joining dataflow connections for producers
 * and consumer. When data is written to the buffers bin will invoke
 * data consumers. Default bin will directly invoke consumers using
 * the current thread without context switches.
 * It is possible to subclass bin for threaded usage where bin instance
 * is run by dedicated thread.
 */
class Bin
{
public:
    class Command;

    /**
     * Constructor.
     */
    Bin();

    /**
     * Destructor
     */
    virtual ~Bin();

    /**
     * Start bin processing. This should be called after all buffers and
     * readers have been joined.
     */
    virtual void start();

    /**
     * Stop bin processing.
     */
    virtual void stop();

    /**
     * Add new data pusher. Pusher callback is set to call the bin.
     *
     * @param pusher pusher.
     * @param name name for the pusher.
     */
    void add(Pusher*     pusher,   const QString& name);

    /**
     * Add new data consumer.
     *
     * @param consumer consumer.
     * @param name name for the consumer.
     */
    void add(Consumer*   consumer, const QString& name);

    /**
     * Add new data filter.
     *
     * @param filter filter.
     * @param name name for the filter.
     */
    void add(FilterBase* filter,   const QString& name);

    /**
     * Establish dataflow connection.
     *
     * @param producerName Name of the producer.
     * @param sourceName Data source name of the producer.
     * @param consumerName Name of the consumer.
     * @param sinkName Name of the data sink of the consumer.
     * @return was connection succesful.
     */
    bool join(const QString& producerName,
              const QString& sourceName,
              const QString& consumerName,
              const QString& sinkName);

    /**
     * De-establish dataflow connection.
     *
     * @param producerName Name of the producer.
     * @param sourceName Data source name of the producer.
     * @param consumerName Name of the consumer.
     * @param sinkName Name of the data sink of the consumer.
     * @return was disconnection succesful.
     */
    bool unjoin(const QString& producerName,
                const QString& sourceName,
                const QString& consumerName,
                const QString& sinkName);

protected:
    /**
     * Pointer to the producer data source.
     *
     * @param producerName Name of the producer.
     * @param sourceName Data source name of the producer.
     * @return source pointer.
     */
    SourceBase* source(const QString& producerName, const QString& sourceName) const;

    /**
     * Pointer to the consumer data sink.
     *
     * @param consumerName Name of the consumer.
     * @param sinkName Name of the data sink of the consumer.
     * @return sink pointer.
     */
    SinkBase*   sink(const QString& consumerName, const QString& sinkName) const;

    /**
     * Pointer to the producer.
     *
     * @param name Name of the producer.
     * @return producer pointer.
     */
    Producer*   producer(const QString& name) const;

    /**
     * Pointer to the consumer.
     *
     * @param name Name of the consumer.
     * @return consumer pointer.
     */
    Consumer*   consumer(const QString& name) const;

private:
    QHash<QString, Pusher*>     pushers_;   /**< Pushers   */
    QHash<QString, Consumer*>   consumers_; /**< Consumers */
    QHash<QString, FilterBase*> filters_;   /**< Filters   */
};

#endif
