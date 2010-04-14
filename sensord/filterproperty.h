/**
   @file filterproperty.h
   @brief Thread safe properties for filters

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
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

#ifndef FILTERPROPERTY_H
#define FILTERPROPERTY_H

#include <QObject>
#include <QAtomicPointer>
#include <QMetaType>
#include <QDebug>

/**
 * @brief Base class for FilterProperty.
 * 
 * Currently no functionality.
 */
class FilterPropertyBase {};

/**
 * @brief Tracker class for FilterProperty.
 *
 * Provides functionality for tracking changes in FilterProperties. \c trackWrite() 
 * function is run when a property value this tracker has been attached to has been 
 * set.
 */
class PropertyTracker
{
public:
    virtual ~PropertyTracker() {}
    
    /**
     * Run when a property value has been set in a FilterProperty this tracker 
     * is monitoring.
     * @param Pointer to the calling FilterPropertyBase.
     */
    virtual void trackWrite(FilterPropertyBase*) {}
};


template <class T>
class FilterProperty;

/**
 * @brief Property class for use in filters and adaptors.
 * 
 * Provides thread safe way of using QProperties in filters and adaptors.
 * See e.g. OrientationInterpreter for sample use.
 */
template <class T>
class FilterProperty : public FilterPropertyBase
{
public:
    /**
     * Constructor. Requires a PropertyTracker to be attached.
     * @param tracker Tracker to attach to this property. Must be valid.
     */
    FilterProperty(PropertyTracker& tracker) :
        future_(),
        current_(new T),
        tracker_(tracker) {}

    /**
     * Accessor function for the property.
     */
    const T& operator()()
    {
        T* p = future_.fetchAndStoreRelaxed(0);
        if (p) {
            delete current_;
            current_ = p;
        }
        return *current_;
    }

    /**
     * Mutator function for the property.
     */
    void operator()(const T& v)
    {
        T* p = new T(v);
        delete future_.fetchAndStoreRelaxed(p);
        tracker_.trackWrite(this);
    }

private:
    QAtomicPointer<T> future_;  /**< Recently set value. About to become current_ */
    T*                current_; /**< Current value */
    PropertyTracker&  tracker_; /**< Tracker for this property */
};

/**
 * @brief See #FilterProperty<T>.
 */
template <class T>
class FilterProperty<T*> : public FilterPropertyBase
{
public:
    FilterProperty(PropertyTracker& tracker) :
        future_(),
        current_(new T),
        tracker_(tracker) {}

    T*& operator()()
    {
        T* p = future_.fetchAndStoreRelaxed(0);
        if (p) {
            delete current_;
            current_ = p;
        }
        return current_;
    }

    void operator()(T*& v)
    {
        T* p = new T(*v);
        delete future_.fetchAndStoreRelaxed(p);
        tracker_.trackWrite(this);
    }
private:
    QAtomicPointer<T> future_;
    T*                current_;
    PropertyTracker&  tracker_;
};

#endif

