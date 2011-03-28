/**
   @file ringbuffer.h
   @brief RingBuffer

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
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

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "consumer.h"
#include "sink.h"
#include "pusher.h"
#include "logging.h"
#include <QSet>

template <class TYPE>
class RingBuffer;

/**
 * Base-class for ring buffer reader subclasses.
 */
class RingBufferReaderBase : public Pusher
{
protected:
    /**
     * Destructor
     */
    virtual ~RingBufferReaderBase();
};

/**
 * Ring buffer reader.
 *
 * @tparam TYPE datatype to reader from buffer.
 */
template <class TYPE>
class RingBufferReader : public RingBufferReaderBase
{
public:
    /**
     * Constructor.
     */
    RingBufferReader() : readCount_(0) {}

    /**
     * Destructor
     */
    virtual ~RingBufferReader() {}

protected:
    /**
     * Read data from buffer.
     *
     * @param n maximum number of objects to read.
     * @param values location to write to.
     * @return how many objects were read.
     */
    unsigned read(unsigned n, TYPE* values)
    {
        return buffer_->read(n, values, *this);
    }

private:
    friend class RingBuffer<TYPE>;

    unsigned                readCount_; /**< how many objects have been read */
    const RingBuffer<TYPE>* buffer_; /**< buffer associated with this reader */
};

/**
 * Base-class fo ring buffers.
 */
class RingBufferBase : public Consumer
{
public:
    /**
     * Destructor.
     */
    virtual ~RingBufferBase() {}

    /**
     * Connect reader to this buffer.
     *
     * @param reader reader to connect.
     * @return was reader connected.
     */
    bool join(RingBufferReaderBase* reader);

    /**
     * Disconnect reader from this buffer.
     *
     * @param reader reader to disconnect.
     * @return was reader disconnected.
     */
    bool unjoin(RingBufferReaderBase* reader);

private:
    /**
     * Connect reader to this buffer.
     *
     * @param reader reader to connect.
     */
    virtual void joinTypeChecked(RingBufferReaderBase* reader) = 0;

    /**
     * Disconnect reader from this buffer.
     *
     * @param reader reader to disconnect.
     */
    virtual void unjoinTypeChecked(RingBufferReaderBase* reader) = 0;
};

/**
 * Ring buffer implementation.
 *
 * @tparam TYPE data type in buffer.
 */
template <class TYPE>
class RingBuffer : public RingBufferBase
{
public:
    /**
     * Constructor.
     *
     * @param size how many elements can be buffered.
     */
    RingBuffer(unsigned size) :
        sink_(this, &RingBuffer::write),
        bufferSize_(size),
        writeCount_()
    {
        buffer_ = new TYPE[size];
        addSink(&sink_, "sink");
    }

    /**
     * Destructor.
     */
    virtual ~RingBuffer()
    {
        delete [] buffer_;
    }

    /**
     * Read data from buffer.
     *
     * @param n how many objects try to read.
     * @param values location to write objects.
     * @param reader buffer reader.
     * @return how many objects were read.
     */
    unsigned read(unsigned                n,
                  TYPE*                   values,
                  RingBufferReader<TYPE>& reader) const
    {
        unsigned itemsRead = 0;
        while (itemsRead < n && reader.readCount_ != (unsigned)writeCount_) {

            *values++ = buffer_[reader.readCount_++ % bufferSize_];

            ++itemsRead;
        }

        return itemsRead;
    }

protected:
    /**
     * Get next slot in the ring buffer.
     *
     * @return next slot.
     */
    TYPE* nextSlot()
    {
        return &buffer_[writeCount_ % bufferSize_];
    }

    /**
     * Called for each object written into buffer.
     */
    void commit()
    {
        ++writeCount_;
    }

    /**
     * Wake up connected buffer readers.
     */
    void wakeUpReaders()
    {
        RingBufferReader<TYPE>* reader;
        foreach (reader, readers_) {
            reader->wakeup();
        }
    }

    /**
     * Write to buffer.
     *
     * @param n how many objects to write.
     * @param values location from where to copy objects.
     */
    void write(unsigned n, const TYPE* values)
    {
        // buffer incoming data
        while (n) {
            *nextSlot() = *values++;
            commit();
            --n;
        }
        wakeUpReaders();
    }

    /**
     * Connect buffer reader to this buffer.
     *
     * @param reader reader to connect.
     */
    virtual void joinTypeChecked(RingBufferReaderBase* reader)
    {
        sensordLogT() << "joining reader to ringbuffer.";

        RingBufferReader<TYPE>* r =
            dynamic_cast<RingBufferReader<TYPE>*>(reader);
        if (r == NULL) {
            sensordLogW() << "Ringbuffer join failed!";
            return;
        }

        r->readCount_ = writeCount_;
        r->buffer_    = this;

        readers_.insert(r);
    }

    /**
     * Disconnect buffer reader from this buffer.
     *
     * @param reader reader to disconnect.
     */
    virtual void unjoinTypeChecked(RingBufferReaderBase* reader)
    {
        RingBufferReader<TYPE>* r =
            dynamic_cast<RingBufferReader<TYPE>*>(reader);
        if (r == NULL) {
            sensordLogW() << "Ringbuffer unjoin failed!";
            return;
        }

        readers_.remove(r);
    }

private:

    Sink<RingBuffer, TYPE>        sink_;       /**< data sink */
    const unsigned                bufferSize_; /**< buffer size */
    TYPE*                         buffer_;     /**< buffer */
    unsigned int                  writeCount_; /**< how many objects have been written */
    QSet<RingBufferReader<TYPE>*> readers_;    /**< connected readers */
};

#endif
