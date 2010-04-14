/**
   @file raworientation.h
   @brief QObject based datatype for raw orientation

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>

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

#ifndef RAW_ORIENTATION_H
#define RAW_ORIENTATION_H

#include <QDBusArgument>

#include "filters/motionbanddata.h"

// TODO: derive from AbstractSensorData
class RawOrientation : public QObject //AbstractSensorData
{
    Q_OBJECT

public:
    // default constructor (needed for Qt metatype system)
    RawOrientation() {}

    // copy constructor (needed for Qt metatype system)
    RawOrientation(const DeviceAxesData& deviceAxesData);
    RawOrientation(const RawOrientation& orientation);

    const DeviceAxesData& deviceAxesData() const { return data_; }

private:
    // TODO: make this a base class
    DeviceAxesData data_;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const RawOrientation& orientation);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, RawOrientation& orientation);
    friend class RawOrientationSensorChannel;
};

Q_DECLARE_METATYPE( RawOrientation )

// Marshall the Orientation data into a D-Bus argument
inline
QDBusArgument &operator<<(QDBusArgument &argument, const RawOrientation &orientation)
{
    argument.beginStructure();
    argument << orientation.data_.x.x << orientation.data_.x.y << orientation.data_.x.z;
    argument << orientation.data_.y.x << orientation.data_.y.y << orientation.data_.y.z;
    argument << orientation.data_.z.x << orientation.data_.z.y << orientation.data_.z.z;
    argument.endStructure();
    return argument;
}

// Retrieve the Orientation data from the D-Bus argument
inline
const QDBusArgument &operator>>(const QDBusArgument &argument, RawOrientation &orientation)
{
    argument.beginStructure();
    argument >> orientation.data_.x.x >> orientation.data_.x.y >> orientation.data_.x.z;
    argument >> orientation.data_.y.x >> orientation.data_.y.y >> orientation.data_.y.z;
    argument >> orientation.data_.z.x >> orientation.data_.z.y >> orientation.data_.z.z;
    argument.endStructure();
    return argument;
}

#endif // RAW_ORIENTATION_H
