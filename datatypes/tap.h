/**
   @file tap.h
   @brief QObject based datatype for TapData

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

#ifndef TAP_H
#define TAP_H

#include <QDBusArgument>

#include <datatypes/tapdata.h>

/**
 * QObject facade for #TapData.
 */
class Tap : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int direction READ direction)
    Q_PROPERTY(int type READ type)

public:
    /**
     * Default constructor.
     */
    Tap() {}

    /**
     * Constructor.
     *
     * @param tapData Source object.
     */
    Tap(const TapData& tapData);

    /**
     * Copy constructor.
     *
     * @param tap Source object.
     */
    Tap(const Tap& tap);

    /**
     * Returns the contained #TapData.
     * @return TapData
     */
    const TapData& tapData() const { return data_; }

    /**
     * Returns tap direction.
     * @return Tap direction
     */
    TapData::Direction direction() const { return data_.direction_; }

    /**
     * Returns tap type.
     * @return Tap type.
     */
    TapData::Type type() const { return data_.type_; }

private:
    TapData data_; /**< Contained tap data */

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Tap& tap);
};

Q_DECLARE_METATYPE( Tap )

/**
 * Marshall the Tap data into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param tap data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const Tap &tap)
{
    argument.beginStructure();
    argument << tap.tapData().timestamp_ << (int)(tap.tapData().direction_) << (int)(tap.tapData().type_);
    argument.endStructure();
    return argument;
}

/**
 * Unmarshall Tap data from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param tap unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, Tap &tap)
{
    int tmp;
    argument.beginStructure();
    argument >> tap.data_.timestamp_;
    argument >> tmp;
    tap.data_.direction_ = (TapData::Direction)tmp;
    argument >> tmp;
    tap.data_.type_ = (TapData::Type)tmp;
    argument.endStructure();
    return argument;
}

#endif // TAP_H
