/**
   @file bufferreader.h
   @brief BufferReader

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   This file is part of Sensord.

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>

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


template <class TYPE>
class BufferReader : public RingBufferReader<TYPE>
{
public:
    BufferReader(unsigned chunkSize) :
        chunkSize_(chunkSize),
        chunk_(new TYPE[chunkSize])
    {
        addSource(&source_, "source");
    }

    ~BufferReader()
    {
        delete[] chunk_;
    }

    void pushNewData()
    {
        unsigned n;
        while ((n = RingBufferReader<TYPE>::read(chunkSize_, chunk_))) {
            source_.propagate(n, chunk_);
        }
    }

private:
    Source<TYPE> source_;
    unsigned     chunkSize_;
    TYPE*        chunk_;
};

#endif
