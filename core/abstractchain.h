/**
   @file abstractchain.h
   @brief Base class for sensor chains

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#ifndef ABSTRACT_CHAIN_H
#define ABSTRACT_CHAIN_H

#include <QMap>

#include "ringbuffer.h"
#include "abstractsensor.h"

/**
 * AbstractChain is a container for filterchain ending in one or more named buffers.
 * It allows sharing of commmon processing easily, without having to rewrite
 * or recalculate filterchains.
 *
 * Due to the idea that processing should be shared, each chain is a chain
 * from HW to processed data. It is not possible to change inputs, as that
 * would make sharing difficult.
 */
class AbstractChain : public AbstractSensorChannel
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractChain)

public:
    /**
     * Destructor
     */
    virtual ~AbstractChain();

    /**
     * Locate a named end buffer of the chain. Buffer names \em should be
     * available from documentation of the given chain.
     *
     * @param name Name of the buffer.
     * @return Pointer to the buffer. If named buffer is not found
     *         NULL is returned.
     */
    RingBufferBase* findBuffer(const QString& name) const;

    /**
     * List of available buffers.
     *
     * @return available buffers.
     */
    const QMap<QString, RingBufferBase*>& buffers() const;

protected:
    /**
     * Constructor.
     *
     * @param id identifier of the chain.
     * @param deleteBuffers delete attached buffers automatically when
     *                      chain is deleted.
     */
    AbstractChain(const QString& id, bool deleteBuffers = false);

    /**
     * Attach buffer with given name.
     *
     * @param name Name of the buffer.
     * @param buffer Pointer to buffer.
     */
    void nameOutputBuffer(const QString& name, RingBufferBase* buffer);

private:
    QMap<QString, RingBufferBase*> outputBufferMap_; /**< buffers */
    const bool deleteBuffers_; /**< are buffers deleted automatically */
};

/**
 * Factory type for constructing chain.
 */
typedef AbstractChain* (*ChainFactoryMethod)(const QString& id);

#endif // ABSTRACT_CHAIN_H
