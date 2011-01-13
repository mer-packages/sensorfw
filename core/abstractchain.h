/**
   @file abstractchain.h
   @brief Base class for sensor chains

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

#ifndef ABSTRACT_CHAIN_H
#define ABSTRACT_CHAIN_H

#include <QMap>

#include "ringbuffer.h"
#include "abstractsensor.h"

/**
 * AbstractChain is a container for filterchain ending in a named buffer.
 * It allows sharing of commmon processing easily, without having to rewrite
 * or recalculate filterchains.
 *
 * Due to the idea that processing should be shared, each chain is a chain
 * from HW to processed data. It is not possible to change inputs, as that
 * would make sharing difficult.
 *
 * @todo Should the interface provide function for listing available buffers?
 */
class AbstractChain : public AbstractSensorChannel
{
    Q_OBJECT;
public:
    virtual ~AbstractChain() {
        // Who owns the output buffers?
    }

    /**
     * Locate a named end buffer of the chain. Buffer names \em should be
     * available from documentation of the given chain.
     * @return Pointer to the buffer with the given name. If named buffer
     *         is not found, \c NULL is returned.
     */
    RingBufferBase* findBuffer(const QString& name) const {
        QMap<QString, RingBufferBase*>::const_iterator it = outputBufferMap_.find(name);
        if (it == outputBufferMap_.end())
            return NULL;
        return it.value();
    }

protected:
    AbstractChain(const QString& id) : AbstractSensorChannel(id) {}

    void nameOutputBuffer(const QString& name, RingBufferBase* buffer) {
        outputBufferMap_.insert(name, buffer);
    }

private:
    QMap<QString, RingBufferBase*> outputBufferMap_;
};

typedef AbstractChain* (*ChainFactoryMethod)(const QString& id);

#endif // ABSTRACT_CHAIN_H
