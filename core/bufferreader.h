/**
   @file bufferreader.h
   @brief BufferReader

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   This file is part of Sensord.

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#ifndef BUFFERREADER_H
#define BUFFERREADER_H

#include "pusher.h"
#include "source.h"
#include "ringbuffer.h"

/**
 * Data producer subclass which reads data from RingBuffer and propagates
 * it into sinks attached into source "source".
 *
 * @tparam TYPE Data type of entries in RingBuffer.
 */
template <class TYPE>
class BufferReader : public RingBufferReader<TYPE>
{
public:
    /**
     * Constructor.
     *
     * @param chunkSize how many objects reader can process with single call
     */
    BufferReader(unsigned chunkSize) :
        chunkSize_(chunkSize),
        chunk_(new TYPE[chunkSize])
    {
        addSource(&source_, "source");
    }

    /**
     * Destructor.
     */
    virtual ~BufferReader()
    {
        delete[] chunk_;
    }

    /**
     * Propagate data into sinks attached to source "source".
     */
    void pushNewData()
    {
        unsigned n;
        while ((n = RingBufferReader<TYPE>::read(chunkSize_, chunk_))) {
            source_.propagate(n, chunk_);
        }
    }

private:
    Source<TYPE> source_;    /**< Source */
    unsigned     chunkSize_; /**< How many objects can be buffered */
    TYPE*        chunk_;     /**< Data storage */
};

#endif
