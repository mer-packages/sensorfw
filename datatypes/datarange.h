/**
   @file datarange.h
   @brief Datatype containing sensor data range and resolution.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#ifndef DATARANGE_H
#define DATARANGE_H

#include <QObject>
#include <QDBusArgument>
#include <QPair>

/* Datatype for storing integer ranges. */
typedef QPair<unsigned int, unsigned int> IntegerRange;

/* Datatype for storing list of integer ranges. */
typedef QList<IntegerRange> IntegerRangeList;

Q_DECLARE_METATYPE( IntegerRange )
Q_DECLARE_METATYPE( IntegerRangeList )

/**
 * Datatype for storing sensor data range information.
 */
class DataRange : public QObject {
    Q_OBJECT;
public:

    /**
     * Default constructor.
     */
    DataRange() : QObject(), min(0), max(0), resolution(0) {}

    /**
     * Copy constructor
     *
     * @param other source object.
     */
    DataRange(const DataRange &other) :
        QObject(), min(other.min), max(other.max), resolution(other.resolution) {}

    /**
     * Constructor.
     *
     * @param min Range lower end.
     * @param max Range higher end.
     * @param resolution Resolution of the range.
     */
    DataRange(double min, double max, double resolution) :
        QObject(), min(min), max(max), resolution(resolution) {}

    /* Range lower end */
    double min;

    /* Range higher end */
    double max;

    /* Range resolution */
    double resolution;

    /**
     * Assignment operator.
     *
     * @param origin Source object for assigment.
     */
    DataRange& operator=(const DataRange& origin)
    {
        min = origin.min;
        max = origin.max;
        resolution = origin.resolution;
        return *this;
    }

    /**
     * Comparison operator.
     *
     * @param right object to compare to.
     * @return comparison result.
     */
    bool operator==(const DataRange& right) const
    {
        return (min == right.min &&
                max == right.max &&
                resolution == right.resolution);
    }
};

/* Datatype for list of data ranges */
typedef QList<DataRange> DataRangeList;

Q_DECLARE_METATYPE( DataRange )
Q_DECLARE_METATYPE( DataRangeList )

/**
 * Marshall the DataRange into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param data data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const DataRange &data)
{
    argument.beginStructure();
    argument << data.min << data.max << data.resolution;
    argument.endStructure();
    return argument;
}

/**
 * Unmarshall DataRange from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param data unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, DataRange &data)
{
    argument.beginStructure();
    argument >> data.min >> data.max >> data.resolution;
    argument.endStructure();
    return argument;
}

/**
 * Marshall the DataRangeList into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param data data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const DataRangeList &data)
{
    argument.beginArray(qMetaTypeId<DataRange>());
    foreach(const DataRange& range, data)
    {
        argument << range;
    }
    argument.endArray();
    return argument;
}

/**
 * Unmarshall DataRangeList from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param data unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, DataRangeList &data)
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

/**
 * Marshall the IntegerRange into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param data data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const IntegerRange &data)
{
    argument.beginStructure();
    argument << data.first << data.second;
    argument.endStructure();
    return argument;
}

/**
 * Unmarshall IntegerRange from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param data unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, IntegerRange &data)
{
    argument.beginStructure();
    argument >> data.first >> data.second;
    argument.endStructure();
    return argument;
}

/**
 * Marshall the IntegerRangeList into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param data data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const IntegerRangeList &data)
{
    argument.beginArray(qMetaTypeId<IntegerRange>());
    foreach(const IntegerRange& range, data)
    {
        argument << range;
    }
    argument.endArray();
    return argument;
}

/**
 * Unmarshall IntegerRangeList from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param data unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, IntegerRangeList &data)
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
 * DataRange request class.
 */
class DataRangeRequest
{
public:

    /* Request ID */
    int       id;

    /* Resuested range */
    DataRange range;

    /**
     * Constructor.
     *
     * @param newId id of the request.
     */
    DataRangeRequest(int newId) :
        id(newId) {};

    /**
     * Constructor.
     *
     * @param newId id of the request.
     * @param newRange requested range.
     */
    DataRangeRequest(int newId, const DataRange& newRange) :
        id(newId),
        range(newRange) {};

    /**
     * Comparison operator.
     *
     * @param right object to compare to.
     * @return comparison result.
     */
    bool operator==(const DataRangeRequest& right) const
    {
        return (id == right.id && range == right.range);
    }
};

/**
 * Interval Request class.
 */
class IntervalRequest {
public:
    /* Request ID */
    int      id;

    /* Requested interval value */
    unsigned value;

    /**
     * Constructor.
     *
     * @param newId Request ID.
     * @param newValue interval value to request.
     */
    IntervalRequest(int newId, unsigned newValue) :
        id(newId),
        value(newValue) {}

    /**
     * Comparison operator.
     *
     * @param right object to compare to.
     * @return comparison result.
     */
    bool operator==(const IntervalRequest& right) const
    {
        return (id == right.id && value == right.value);
    }
};

/**
 * Checks is given value inside range list.
 *
 * @param ref value to check.
 * @param container range list.
 * @return is value in range or not.
 */
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
