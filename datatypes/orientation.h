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

#include <QDBusArgument>
#include <datatypes/orientationdata.h>

/**
 * QObject facade for #OrientationData.
 */
class Orientation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int x READ x)
    Q_PROPERTY(int y READ y)
    Q_PROPERTY(int z READ z)
    Q_PROPERTY(DisplayOrientation orientation READ orientation)

public:
    /**
     * Display orientation.
     *
     * @deprecated Not used.
     */
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

    /**
     * Default constructor.
     */
    Orientation() {}

    /**
     * Constructor.
     *
     * @param orientationData Source object.
     */
    Orientation(const OrientationData& orientationData);

    /**
     * Copy constructor.
     *
     * @param orientation Source object.
     */
    Orientation(const Orientation& orientation);

    /**
     * Accessor for contained #OrientationData.
     *
     * @return contained #OrientationData.
     */
    const OrientationData& orientationData() const { return data_; }

    /**
     * Accessor for X coordinate.
     *
     * @return X coordinate.
     */
    int x() const { return data_.x_; }

    /**
     * Accessor for Y coordinate.
     *
     * @return Y coordinate.
     */
    int y() const { return data_.y_; }

    /**
     * Accessor for Z coordinate.
     *
     * @return Z coordinate.
     */
    int z() const { return data_.z_; }

    /**
     * Accessor for display orientation.
     *
     * @return display orientation.
     *
     * @deprecated Returns always OrientationUndefined.
     */
    DisplayOrientation orientation() const { return OrientationUndefined; }

private:
    OrientationData data_; /**< Contained data */

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Orientation& orientation);
};

Q_DECLARE_METATYPE( Orientation )

/**
 * Marshall the Orientation data into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param orientation data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const Orientation &orientation)
{
    argument.beginStructure();
    argument << orientation.orientationData().timestamp_ << orientation.orientationData().x_ << orientation.orientationData().y_ << orientation.orientationData().z_;
    argument.endStructure();
    return argument;
}

/**
 * Unmarshall Orientation data from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param orientation unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, Orientation &orientation)
{
    argument.beginStructure();
    argument >> orientation.data_.timestamp_ >> orientation.data_.x_ >> orientation.data_.y_ >> orientation.data_.z_;
    argument.endStructure();
    return argument;
}

#endif // ORIENTATION_H
