/**
   @file source.h
   @brief Source

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

#ifndef SOURCE_H
#define SOURCE_H

#include "sink.h"
#include "logging.h"
#include <typeinfo>
#include <QSet>

class SinkBase;

/**
 * Base-class for data source.
 */
class SourceBase
{
public:
    /**
     * Connect sink to the source.
     *
     * @param sink Sink.
     * @return was sink connected succesfully.
     */
    bool join(SinkBase* sink);

    /**
     * Disconnect sink from the source.
     *
     * @param sink Sink.
     * @return was sink disconnected succesfully.
     */
    bool unjoin(SinkBase* sink);

protected:
    /**
     * Destructor.
     */
    virtual ~SourceBase() {}

private:
    /**
     * Connect and check that sink is compatible with source.
     *
     * @param sink Sink.
     * @return was sink joined.
     */
    virtual bool joinTypeChecked(SinkBase* sink) = 0;

    /**
     * Disconnect and check that sink is compatible with source.
     *
     * @param sink Sink.
     * @return was sink unjoined.
     */
    virtual bool unjoinTypeChecked(SinkBase* sink) = 0;
};

/**
 * Data source.
 *
 * @tparam TYPE type of data streamed from the source.
 */
template <class TYPE>
class Source : public SourceBase
{
public:
    /**
     * Propagate data to connected sinks.
     *
     * @param n how many elements to stream.
     * @param values source from where to stream data.
     */
    void propagate(int n, const TYPE* values)
    {
        foreach (SinkTyped<TYPE>* sink, sinks_) {
            sink->collect(n, values);
        }
    }
private:
    bool joinTypeChecked(SinkBase* sink)
    {
        SinkTyped<TYPE>* type = dynamic_cast<SinkTyped<TYPE>*>(sink);
        if(type)
        {
            sinks_.insert(type);
            return true;
        }
        sensordLogC() << "Failed to join type '" << typeid(type).name() << " to source!";
        return false;
    }

    bool unjoinTypeChecked(SinkBase* sink)
    {
        SinkTyped<TYPE>* type = dynamic_cast<SinkTyped<TYPE>*>(sink);
        if(type)
        {
            sinks_.remove(type);
            return true;
        }
        sensordLogC() << "Failed to unjoin type '" << typeid(type).name() << " from source!";
        return false;
    }

    QSet<SinkTyped<TYPE>*> sinks_; /**< connected sinks. */
};

#endif
