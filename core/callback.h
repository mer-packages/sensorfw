/**
   @file callback.h
   @brief Callback

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   This file is part of Sensord.

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>

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

class CallbackBase
{
public:
    virtual void operator()() const = 0;

protected:
    virtual ~CallbackBase() {}
};

template <class T>
class Callback : public CallbackBase
{
private:
    typedef void (T::* Member)();

public:
    Callback(T* instance, Member member) :
        instance_(instance),
        member_(member)
    {}

    void operator()() const
    {
        (instance_->*member_)();
    }

private:
    T*     instance_;
    Member member_;
};

#if 0
template <class T, class P1, class P2>
class Callback2
{
private:
    typedef void (T::* Member)(P1, P2);

public:
    Callback2(T* instance, Member member) :
        instance_(instance),
        member_(member)
    {}

    void operator()(P1 p1, P2 p2) const
    {
        (instance_->*member_)(p1, p2);
    }

private:
    T*     instance_;
    Member member_;
};
#endif

#endif
