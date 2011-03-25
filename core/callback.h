/**
   @file callback.h
   @brief Callback

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   This file is part of Sensord.

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#ifndef CALLBACK_H
#define CALLBACK_H

/**
 * Baseclass of invokable callback.
 */
class CallbackBase
{
public:
    /**
     * Invoke callback
     */
    virtual void operator()() const = 0;

protected:
    /**
     * Destructor.
     */
    virtual ~CallbackBase() {}
};

/**
 * Invokable callback.
 *
 * @tparam T class which implements callback.
 */
template <class T>
class Callback : public CallbackBase
{
private:
    /**
     * Callback function.
     */
    typedef void (T::* Member)();

public:
    /**
     * Constructor.
     *
     * @param instance Class instance with callback.
     * @param member Class function implementing callback.
     */
    Callback(T* instance, Member member) :
        instance_(instance),
        member_(member)
    {}

    /**
     * Invoke callback.
     */
    void operator()() const
    {
        (instance_->*member_)();
    }

private:
    T*     instance_; /**< Class instance with callback. */
    Member member_;   /**< Class function implementing callback */
};

#endif
