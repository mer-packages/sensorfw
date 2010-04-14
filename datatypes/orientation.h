/**
   @file orientation.h
   @brief QObject based datatype for OrientationData

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
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

#ifndef ORIENTATION_H
#define ORIENTATION_H

#include <QtDebug>
#include <QDBusArgument>

#include <filters/orientationdata.h>

// TODO: derive from AbstractSensorData
class Orientation : public QObject //AbstractSensorData
{
    Q_OBJECT

    Q_PROPERTY(int x READ x)
    Q_PROPERTY(int y READ y)
    Q_PROPERTY(int z READ z)
    Q_PROPERTY(DisplayOrientation orientation READ orientation)

public:
    enum DisplayOrientation
    {
        OrientationUndefined = 0,
        OrientationDisplayUp,
        OrientationDisplayDown,
        OrientationDisplayLeftUp,
        OrientationDisplayRightUp,
        OrientationDisplayUpwards,
        OrientationDisplayDownwards
    };

    // default constructor (needed for Qt metatype system)
    Orientation() {}

    // copy constructor (needed for Qt metatype system)
    Orientation(const OrientationData& orientationData);
    Orientation(const Orientation& orientation);

    const OrientationData& orientationData() const { return  data_; }

    int x() const { return data_.x_; }
    int y() const { return data_.y_; }
    int z() const { return data_.z_; }

    DisplayOrientation orientation() const { return OrientationUndefined; }

private:
    // TODO: make this a base class
    OrientationData data_;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const Orientation& orientation);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Orientation& orientation);
    friend class OrientationSensorChannel;
};

Q_DECLARE_METATYPE( Orientation )

// Marshall the Orientation data into a D-Bus argument
inline
QDBusArgument &operator<<(QDBusArgument &argument, const Orientation &orientation)
{
    argument.beginStructure();
    argument << orientation.data_.timestamp_ << orientation.data_.x_ << orientation.data_.y_ << orientation.data_.z_;
    argument.endStructure();
    return argument;
}

// Retrieve the Orientation data from the D-Bus argument
inline
const QDBusArgument &operator>>(const QDBusArgument &argument, Orientation &orientation)
{
    argument.beginStructure();
    argument >> orientation.data_.timestamp_ >> orientation.data_.x_ >> orientation.data_.y_ >> orientation.data_.z_;
    argument.endStructure();
    return argument;
}

#endif // ORIENTATION_H
