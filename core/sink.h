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

/**
 * Data sink base class.
 */
class SinkBase
{
protected:
    /**
     * Destructor.
     */
    virtual ~SinkBase() {}
};

/**
 * Data sink inteface with data type.
 *
 * @tparam TYPE Data type which sink accepts.
 */
template <class TYPE>
class SinkTyped : public SinkBase
{
public:
    /**
     * Push data to sink.
     *
     * @param n Number of elements.
     * @param values Data source location.
     */
    virtual void collect(int n, const TYPE* values) = 0;
};

/**
 * Data sink.
 *
 * @tparam DERIVED Data sink implementor type.
 * @tparam TYPE Data type which sink accepts.
 */
template <class DERIVED, class TYPE>
class Sink : public SinkTyped<TYPE>
{
public:
    /**
     * Sink callback function type.
     */
    typedef void (DERIVED::* Member)(unsigned n, const TYPE* values);

    /**
     * Constructor.
     *
     * @param instance implementor reference.
     * @param member callback function reference.
     */
    Sink(DERIVED* instance, Member member) :
        instance_(instance),
        member_(member)
    {}

private:
    void collect(int n, const TYPE* values)
    {
        (instance_->*member_)(n, values);
    }

    DERIVED* instance_; /** sink callback implementor */
    Member   member_;   /** sink callback function */
};

#endif
