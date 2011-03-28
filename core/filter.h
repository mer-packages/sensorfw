/**
   @file filter.h
   @brief Base class for filters

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>

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

#ifndef FILTER_H
#define FILTER_H

#include "consumer.h"
#include "producer.h"
#include "sink.h"
#include "source.h"

/**
 * Filter base class.
 */
class FilterBase : public Consumer, public Producer
{
protected:
    /**
     * Default constructor.
     */
    FilterBase();
};

/**
 * Extendable filter class. Filters data from given source "source"
 * to sink "sink".
 *
 * @tparam INPUT_TYPE input data type.
 * @tparam DERIVED subclass type.
 * @tparam OUTPUT_TYPE output data type.
 */
template <class INPUT_TYPE, class DERIVED, class OUTPUT_TYPE = INPUT_TYPE>
class Filter : public FilterBase
{
public:
    /**
     * Constructor.
     *
     * @param instance pointer to subclass instance.
     * @param member sink callback.
     */
    Filter(DERIVED*                                   instance,
           typename Sink<DERIVED, INPUT_TYPE>::Member member) :
        sink_(instance, member)
    {
        addSink(&sink_, "sink");
        addSource(&source_, "source");
    }

protected:
    Sink<DERIVED, INPUT_TYPE> sink_;   /**< data sink.   */
    Source<OUTPUT_TYPE>       source_; /**< data source. */
};

/**
 * Filter factory definition.
 */
typedef FilterBase* (*FilterFactoryMethod)();

#endif
