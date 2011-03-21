/**
   @file unsigned.h
   @brief QObject based datatype for TimedUnsigned

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

#ifndef UNSIGNED_H
#define UNSIGNED_H

#include <QDBusArgument>
#include <datatypes/timedunsigned.h>

/**
 * QObject facae for #TimedUnsigned.
 */
class Unsigned : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x)

public:

    /**
     * Default constructor.
     */
    Unsigned() {}

    /**
     * Constructor.
     *
     * @param unsignedData Source object.
     */
    Unsigned(const TimedUnsigned& unsignedData);

    /**
     * Copy constructor.
     *
     * @param data Source object.
     */
    Unsigned(const Unsigned& data);

    /**
     * Returns the contained #TimedUnsigned
     * @return Contained TimedUnsigned
     */
    const TimedUnsigned& UnsignedData() const { return data_; }

    /**
     * Returns the value.
     * @return value.
     */
    int x() const { return data_.value_; }

    /**
     * Assignment operator.
     *
     * @param origin Source object for assigment.
     */
    Unsigned& operator=(const Unsigned& origin)
    {
        data_ = origin.UnsignedData();
        return *this;
    }

    /**
     * Comparison operator.
     *
     * @param right Object to compare to.
     * @return comparison result.
     */
    bool operator==(const Unsigned& right) const
    {
        TimedUnsigned rdata = right.UnsignedData();
        return (data_.value_ == rdata.value_ &&
                data_.timestamp_ == rdata.timestamp_);
    }

private:
    TimedUnsigned data_; /**< Contained data. */

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Unsigned& data);
};

Q_DECLARE_METATYPE( Unsigned )

/**
 * Marshall the Unsigned data into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param data data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const Unsigned &data)
{
    argument.beginStructure();
    argument << data.UnsignedData().timestamp_ << data.UnsignedData().value_;
    argument.endStructure();
    return argument;
}

/**
 * Unmarshall Unsigned data from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param data unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, Unsigned &data)
{
    argument.beginStructure();
    argument >> data.data_.timestamp_ >> data.data_.value_;
    argument.endStructure();
    return argument;
}

#endif // UNSIGNED_H
