/**
   @file key.h
   @brief QObject based datatype for TimedKey

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

#ifndef KEY_H
#define KEY_H

#include <QDBusArgument>

#include "timedkey.h"

class Key : public QObject //AbstractSensorData
{
    Q_OBJECT

    Q_PROPERTY(uint16_t key READ key)
    Q_PROPERTY(bool pressed READ pressed)
    Q_PROPERTY(bool released READ released)

public:

    /**
     * Default constructor (needed for Qt metatype system)
     */
    Key() {}

    /**
     * Copy constructor (needed for Qt metatype system)
     */
    Key(const TimedKey& unsignedData);

    /**
     * Copy constructor (needed for Qt metatype system)
     */
    Key(const Key& data);

    /**
     * Returns the contained #TimedKey
     * @return Contained TimedKey
     */
    const TimedKey& KeyData() const { return data_; }

    /**
     * Returns the value.
     * @return value.
     */
    uint16_t key() const { return data_.value_.key; }
    bool pressed() const { return data_.value_.pressed; }
    bool released() const { return data_.value_.released; }

private:
    // TODO: make this a base class
    TimedKey data_;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const Key& data);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Key& data);
};

Q_DECLARE_METATYPE( Key )

// Marshall the Key data into a D-Bus argument
inline
QDBusArgument &operator<<(QDBusArgument &argument, const Key &data)
{
    argument.beginStructure();
    argument << data.data_.timestamp_ << data.key() << data.pressed() << data.released();
    argument.endStructure();
    return argument;
}

// Retrieve the Key data from the D-Bus argument
inline
const QDBusArgument &operator>>(const QDBusArgument &argument, Key &data)
{
    argument.beginStructure();
    argument >> data.data_.timestamp_ >> data.data_.value_.key >> data.data_.value_.pressed >> data.data_.value_.released;
    argument.endStructure();
    return argument;
}

#endif // KEY_H
