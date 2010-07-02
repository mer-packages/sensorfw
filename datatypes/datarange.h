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

Q_DECLARE_METATYPE( DataRange )

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

/**
 * Request class. Contains id of the requester and the range.
 */
class DataRangeRequest {
public:
    int       id_;
    DataRange range_;

    bool operator==(const DataRangeRequest& right) const
    {
        return (id_ == right.id_ && range_ == right.range_);
    }
};

#endif // DATARANGE_H
