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
#include <QSet>

class SinkBase;

class SourceBase
{
public:
    bool join(SinkBase* sink);
    bool unjoin(SinkBase* sink);

protected:
    virtual ~SourceBase() {}

private:
    virtual void joinTypeChecked(SinkBase* sink) = 0;
    virtual void unjoinTypeChecked(SinkBase* sink) = 0;
};


template <class TYPE>
class Source : public SourceBase
{
public:
    void propagate(int n, const TYPE* values)
    {
        SinkTyped<TYPE>* sink;

        foreach (sink, sinks_) {
            sink->collect(n, values);
        }
    }
private:
    void joinTypeChecked(SinkBase* sink)
    {
        sinks_.insert(dynamic_cast<SinkTyped<TYPE>*>(sink));
    }

    void unjoinTypeChecked(SinkBase* sink) {
        sinks_.remove(dynamic_cast<SinkTyped<TYPE>*>(sink));
    }

    QSet<SinkTyped<TYPE>*> sinks_;
};

#endif
