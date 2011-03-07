/**
   @file abstractchain.cpp
   @brief AbstractChain

   <p>
   Copyright (C) 2011 Nokia Corporation

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

#include "abstractchain.h"

AbstractChain::AbstractChain(const QString& id, bool deleteBuffers) :
    AbstractSensorChannel(id),
    deleteBuffers_(deleteBuffers)
{
}

AbstractChain::~AbstractChain()
{
    if(deleteBuffers_)
    {
        foreach(RingBufferBase* buffer, outputBufferMap_.values())
        {
            delete buffer;
        }
    }
}

RingBufferBase* AbstractChain::findBuffer(const QString& name) const
{
    QMap<QString, RingBufferBase*>::const_iterator it = outputBufferMap_.find(name);
    if (it == outputBufferMap_.end())
        return NULL;
    return it.value();
}

void AbstractChain::nameOutputBuffer(const QString& name, RingBufferBase* buffer)
{
    outputBufferMap_.insert(name, buffer);
}

const QMap<QString, RingBufferBase*>& AbstractChain::buffers() const
{
    return outputBufferMap_;
}
