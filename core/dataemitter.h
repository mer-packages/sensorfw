/**
   @file dataemitter.h
   @brief Data emitter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
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

#ifndef DATAEMITTER_H
#define DATAEMITTER_H

#include "pusher.h"
#include "ringbuffer.h"

/**
 * Data producer subclass which emits individual objects. Does not have
 * any sources provided.
 *
 * @tparam TYPE datatype being emitted.
 */
template <class TYPE>
class DataEmitter : public RingBufferReader<TYPE>
{
public:
    /**
     * Constructor.
     *
     * @param chunkSize how many objects will be internally buffered.
     */
    DataEmitter(unsigned chunkSize) :
        chunkSize_(chunkSize),
        chunk_(new TYPE[chunkSize])
    {
    }

    /**
     * Destructor.
     */
    virtual ~DataEmitter()
    {
        delete[] chunk_;
    }

    /**
     * Propagate data by calling emitData.
     */
    void pushNewData()
    {
        unsigned n;
        while ((n = RingBufferReader<TYPE>::read(chunkSize_, chunk_))) {
            for (unsigned i = 0; i < n; ++i) {
                emitData(chunk_[i]);
            }
        }
    }

protected:
    /**
     * Callback for emitted objects.
     */
    virtual void emitData(const TYPE& value) = 0;

private:
    unsigned     chunkSize_; /**< How many objects can be buffered */
    TYPE*        chunk_;     /**< Buffer */
};

#endif
