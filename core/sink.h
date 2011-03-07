/**
   @file sink.h
   @brief Sink

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

#ifndef SINK_H
#define SINK_H

class SinkBase
{
protected:
    virtual ~SinkBase() {}
};

template <class TYPE>
class SinkTyped : public SinkBase
{
public:
    virtual void collect(int n, const TYPE* values) = 0;
};

template <class DERIVED, class TYPE>
class Sink : public SinkTyped<TYPE>
{
public:
    typedef void (DERIVED::* Member)(unsigned n, const TYPE* values);

    Sink(DERIVED* instance, Member member) :
        instance_(instance),
        member_(member)
    {}

private:
    void collect(int n, const TYPE* values)
    {
        (instance_->*member_)(n, values);
    }

    DERIVED* instance_;
    Member   member_;
};

#endif
