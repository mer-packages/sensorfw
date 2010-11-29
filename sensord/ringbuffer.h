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

class RingBufferReaderBase : public Pusher
{
public:
    virtual int typeId() = 0;

protected:
    virtual ~RingBufferReaderBase() {}
};

template <class TYPE>
class RingBufferReader : public RingBufferReaderBase
{
friend class RingBuffer<TYPE>;

public:
    RingBufferReader() : readCount_(0) {}
    virtual ~RingBufferReader() {};

    int typeId() { return 0; } // TODO

protected:
    unsigned read(unsigned n, TYPE* values)
    {
        return buffer_->read(n, values, *this);
    }

private:
    unsigned                readCount_;
    const RingBuffer<TYPE>* buffer_;
};


class RingBufferBase : public Consumer
{
public:
    virtual int typeId() = 0;

    bool join(RingBufferReaderBase* reader);
    bool unjoin(RingBufferReaderBase* reader);

protected:
    virtual ~RingBufferBase() {}

private:
    virtual void joinTypeChecked(RingBufferReaderBase* reader) = 0;
    virtual void unjoinTypeChecked(RingBufferReaderBase* reader) = 0;
};

template <class TYPE>
class RingBuffer : public RingBufferBase
{
public:
    RingBuffer(unsigned size) :
        sink_(this, &RingBuffer::write),
        bufferSize_(size),
        writeCount_()
    {
        buffer_ = new TYPE[size];
        addSink(&sink_, "sink");
    }

    virtual ~RingBuffer()
    {
        delete [] buffer_;
    }

    int typeId() { return 0; } // TODO

    virtual unsigned read(unsigned                n,
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
    TYPE* nextSlot()
    {
        return &buffer_[writeCount_ % bufferSize_];
    }

    void commit()
    {
        ++writeCount_;
    }

    void wakeUpReaders()
    {
        RingBufferReader<TYPE>* reader;
        foreach (reader, readers_) {
            reader->wakeup();
        }
    }

    void write(unsigned n, const TYPE* values)
    {
        // buffer incoming data
        if(__is_pod(TYPE) && n <= bufferSize_) {
            memcpy(nextSlot(), values, n);
            writeCount_ += n;
        }
        else {
            while (n) {
                *nextSlot() = *values++;
                commit();
                --n;
            }
        }
        wakeUpReaders();
    }

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

    Sink<RingBuffer, TYPE>        sink_;
    const unsigned                bufferSize_;
    TYPE*                         buffer_;
    unsigned int                  writeCount_;
    QSet<RingBufferReader<TYPE>*> readers_;
};

#endif
