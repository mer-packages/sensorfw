/**
   @file xyz.h
   @brief QObject based datatype for TimedXYZData

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

#ifndef XYZ_H
#define XYZ_H

#include <QDBusArgument>

#include <datatypes/orientationdata.h>

/**
 * @brief XYZ is a wrapper class for #TimedXYZData.
 *
 * It is used to marshall measurement data with 3 integers and timestamp
 * #TimedXYZData over DBus.
 */
class XYZ : public QObject
{
public:
    Q_OBJECT;

    Q_PROPERTY(int x READ x);
    Q_PROPERTY(int y READ y);
    Q_PROPERTY(int z READ z);

public:

    /**
     * Default constructor (needed for Qt metatype system)
     */
    XYZ() {}

    /**
     * Copy constructor (needed for Qt metatype system)
     */
    XYZ(const TimedXyzData& xyzData);

    /**
     * Copy constructor (needed for Qt metatype system)
     */
    XYZ(const XYZ& xyz);

    /**
     * Returns the contained #TimedXYZData
     * @return Contained TimedXYZData
     */
    const TimedXyzData& XYZData() const { return data_; }

    /**
     * Returns the value for X.
     * @return x value.
     */
    int x() const { return data_.x_; }

    /**
     * Returns the value for Y.
     * @return y value.
     */
    int y() const { return data_.y_; }

    /**
     * Returns the value for Z.
     * @return z value.
     */
    int z() const { return data_.z_; }

    XYZ& operator=(const XYZ& origin)
    {
        data_ = origin.XYZData();
        return *this;
    }

    bool operator==(const XYZ& right) const
    {
        TimedXyzData rdata = right.XYZData();
        return (data_.x_ == rdata.x_ &&
                data_.y_ == rdata.y_ &&
                data_.z_ == rdata.z_ &&
                data_.timestamp_ == rdata.timestamp_);
    }

private:
    TimedXyzData data_;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const XYZ& xyz);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, XYZ& xyz);
    friend class OrientationSensorChannel;
};

Q_DECLARE_METATYPE( XYZ )

// Marshall the XYZ data into a D-Bus argument
inline QDBusArgument &operator<<(QDBusArgument &argument, const XYZ &xyz)
{
    argument.beginStructure();
    argument << xyz.data_.timestamp_ << xyz.data_.x_ << xyz.data_.y_ << xyz.data_.z_;
    argument.endStructure();
    return argument;
}

// Retrieve the XYZ data from the D-Bus argument
inline const QDBusArgument &operator>>(const QDBusArgument &argument, XYZ &xyz)
{
    argument.beginStructure();
    argument >> xyz.data_.timestamp_ >> xyz.data_.x_ >> xyz.data_.y_ >> xyz.data_.z_;
    argument.endStructure();
    return argument;
}

#endif // XYZ_H
