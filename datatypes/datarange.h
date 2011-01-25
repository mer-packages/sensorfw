/**
   @file datarange.h
   @brief Datatype containing sensor data range and resolution.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#ifndef DATARANGE_H
#define DATARANGE_H

#include <QObject>
#include <QDBusArgument>
#include <QPair>

typedef QPair<unsigned int, unsigned int> IntegerRange;
typedef QList<IntegerRange> IntegerRangeList;

Q_DECLARE_METATYPE( IntegerRange )
Q_DECLARE_METATYPE( IntegerRangeList )

/**
 * Datatype for storing sensor data range information.
 */
class DataRange : public QObject {
    Q_OBJECT;
public:

    DataRange() : QObject(), min(0), max(0), resolution(0) {}

    DataRange(const DataRange &other) :
        QObject(), min(other.min), max(other.max), resolution(other.resolution) {}

    DataRange(double pmin, double pmax, double presolution) :
        QObject(), min(pmin), max(pmax), resolution(presolution) {}


    double min;
    double max;
    double resolution;

    DataRange& operator=(const DataRange& origin)
    {
        min = origin.min;
        max = origin.max;
        resolution = origin.resolution;
        return *this;
    }

    bool operator==(const DataRange& right) const
    {
        return (min == right.min &&
                max == right.max &&
                resolution == right.resolution);
    }

};

typedef QList<DataRange> DataRangeList;

Q_DECLARE_METATYPE( DataRange )
Q_DECLARE_METATYPE( DataRangeList )

// Marshall the DataRange data into a D-Bus argument
inline
QDBusArgument &operator<<(QDBusArgument &argument, const DataRange &data)
{
    argument.beginStructure();
    argument << data.min << data.max << data.resolution;
    argument.endStructure();
    return argument;
}

// Retrieve the DataRange data from the D-Bus argument
inline
const QDBusArgument &operator>>(const QDBusArgument &argument, DataRange &data)
{
    argument.beginStructure();
    argument >> data.min >> data.max >> data.resolution;
    argument.endStructure();
    return argument;
}

inline
QDBusArgument &operator<<(QDBusArgument &argument, const DataRangeList &data)
{
    argument.beginArray(qMetaTypeId<DataRange>());
    foreach(const DataRange& range, data)
    {
        argument << range;
    }
    argument.endArray();
    return argument;
}

inline
const QDBusArgument &operator>>(const QDBusArgument &argument, DataRangeList &data)
{
    argument.beginArray();
    data.clear();
    while ( !argument.atEnd() ) {
        DataRange element;
        argument >> element;
        data.append( element );
    }
    argument.endArray();
    return argument;
}

inline
QDBusArgument &operator<<(QDBusArgument &argument, const IntegerRange &data)
{
    argument.beginStructure();
    argument << data.first << data.second;
    argument.endStructure();
    return argument;
}

inline
const QDBusArgument &operator>>(const QDBusArgument &argument, IntegerRange &data)
{
    argument.beginStructure();
    argument >> data.first >> data.second;
    argument.endStructure();
    return argument;
}

inline
QDBusArgument &operator<<(QDBusArgument &argument, const IntegerRangeList &data)
{
    argument.beginArray(qMetaTypeId<IntegerRange>());
    foreach(const IntegerRange& range, data)
    {
        argument << range;
    }
    argument.endArray();
    return argument;
}

inline
const QDBusArgument &operator>>(const QDBusArgument &argument, IntegerRangeList &data)
{
    argument.beginArray();
    data.clear();
    while ( !argument.atEnd() ) {
        IntegerRange element;
        argument >> element;
        data.append( element );
    }
    argument.endArray();
    return argument;
}

/**
 * Request class. Contains id of the requester and the range.
 */
class DataRangeRequest {
public:
    int       id;
    DataRange range;

    DataRangeRequest(int newId) :
        id(newId) {};

    DataRangeRequest(int newId, const DataRange& newRange) :
        id(newId),
        range(newRange) {};

    bool operator==(const DataRangeRequest& right) const
    {
        return (id == right.id && range == right.range);
    }
};

/**
 * Request class for Intervals. Contains id of the requester and value.
 */
class IntervalRequest {
public:
    int      id;
    unsigned value;

    IntervalRequest(int newId, unsigned newValue) :
        id(newId),
        value(newValue) {};

    bool operator==(const IntervalRequest& right) const
    {
        return (id == right.id && value == right.value);
    }
};

template<typename T, typename U>
inline bool isInRange(T ref, const U& container)
{
    foreach(const typename U::value_type& value, container)
    {
        if(ref >= value.first && ref <= value.second)
            return true;
    }
    return false;
}

#endif // DATARANGE_H
