/**
   @file compass.h
   @brief QObject based datatype for CompassData

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

#ifndef COMPASS_H
#define COMPASS_H

#include <QDBusArgument>

#include <datatypes/orientationdata.h>

/**
 * @brief Compass is a wrapper class for #CompassData.
 *
 * It is used for marshalling compass measurements over DBus.
 */
class Compass : public QObject
{
public:
    Q_OBJECT;

    Q_PROPERTY(int degrees READ degrees);
    Q_PROPERTY(int level READ level);

public:

    /**
     * Default constructor (needed for Qt metatype system)
     */
    Compass() {}

    /**
     * Copy constructor (needed for Qt metatype system)
     *
     * @param data Source object.
     */
    Compass(const CompassData& data);

    /**
     * Copy constructor (needed for Qt metatype system)
     *
     * @param data Source object.
     */
    Compass(const Compass& data);

    /**
     * Returns the contained #CompassData
     * @return Contained CompassData
     */
    const CompassData& data() const { return data_; }

    /**
     * Returns the value for degrees.
     * @return degree value.
     */
    int degrees() const { return data_.degrees_; }

    /**
     * Returns the value for level.
     * @return level value.
     */
    int level() const { return data_.level_; }

    /**
     * Assignment operator.
     *
     * @param origin Source object for assigment.
     */
    Compass& operator=(const Compass& origin)
    {
        data_ = origin.data();
        return *this;
    }

    /**
     * Comparison operator.
     *
     * @param right Object to compare to.
     * @return comparison result.
     */
    bool operator==(const Compass& right) const
    {
        CompassData rdata = right.data();
        return (data_.degrees_ == rdata.degrees_ &&
                data_.level_ == rdata.level_ &&
                data_.timestamp_ == rdata.timestamp_);
    }

private:
    CompassData data_;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const Compass& data);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Compass& data);
    friend class CompassSensorChannel;
};

Q_DECLARE_METATYPE( Compass )

/**
 * Marshall the Compass data into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param data data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const Compass &data)
{
    argument.beginStructure();
    argument << data.data_.timestamp_ << data.data_.degrees_ << data.data_.level_;
    argument.endStructure();
    return argument;
}

/**
 * Unmarshall Compass data from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param data unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, Compass &data)
{
    argument.beginStructure();
    argument >> data.data_.timestamp_ >> data.data_.degrees_ >> data.data_.level_;
    argument.endStructure();
    return argument;
}

#endif // COMPASS_H
