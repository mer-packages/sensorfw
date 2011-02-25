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

#include <QtDebug>
#include <QDBusArgument>

#include <datatypes/tapdata.h>

/**
 * @brief Container class for #TapData.
 *
 * @todo Derive from AbstractSensorData
 * @todo Make sure that users of this class don't refer to data_ directly and 
 * make it private.
 */
class Tap : public QObject //AbstractSensorData
{
    Q_OBJECT

    Q_PROPERTY(int direction READ direction)
    Q_PROPERTY(int type READ type)

public:
    /**
     * Default constructor (needed for Qt metatype system)
     */
    Tap() {}

    /**
     * Copy constructor (needed for Qt metatype system)
     */
    Tap(const TapData& tapData);

    /**
     * Copy constructor (needed for Qt metatype system)
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

    TapData data_;

private:

    // TODO: make this a base class    
    friend QDBusArgument &operator<<(QDBusArgument &argument, const Tap& tap);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Tap& tap);
};

Q_DECLARE_METATYPE( Tap )

// Marshall Tap data into a D-Bus argument
inline
QDBusArgument &operator<<(QDBusArgument &argument, const Tap &tap)
{
    argument.beginStructure();
    argument << tap.data_.timestamp_ << (int)(tap.data_.direction_) << (int)(tap.data_.type_);
    argument.endStructure();
    return argument;
}

// Retrieve Tap data from the D-Bus argument
inline
const QDBusArgument &operator>>(const QDBusArgument &argument, Tap &tap)
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
