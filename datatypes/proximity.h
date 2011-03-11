/**
   @file proximity.h
   @brief QObject based datatype for ProximityData

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#ifndef PROXIMITY_H
#define PROXIMITY_H

#include <QDBusArgument>

#include <datatypes/unsigned.h>
#include <datatypes/orientationdata.h>

class Proximity : public Unsigned
{
    Q_OBJECT

    Q_PROPERTY(int reflectance READ reflectance)
    Q_PROPERTY(int withinProximity READ withinProximity)

public:

    /**
     * Default constructor (needed for Qt metatype system)
     */
    Proximity() {}

    /**
     * Copy constructor (needed for Qt metatype system)
     */
    Proximity(const ProximityData& data) :
        Unsigned(TimedUnsigned(data.timestamp_, data.withinProximity_)),
        data_(data.timestamp_, data.value_, data.withinProximity_)
        {}

    /**
     * Copy constructor (needed for Qt metatype system)
     */
    Proximity(const Proximity& data) :
        Unsigned(data.UnsignedData()), data_(data.proximityData().timestamp_, data.proximityData().value_, data.proximityData().withinProximity_)
        {}

    /**
     * Returns the contained #ProximityData
     * @return Contained ProximityData
     */
    const ProximityData& proximityData() const { return data_; }

    /**
     * @return is the proximity sensor withinProximity or not
     */
    bool withinProximity() const { return data_.withinProximity_; }

    /**
     * @return proximity reflectance reading
     */
    int reflectance() const { return data_.value_; }

    Proximity& operator=(const Proximity& origin)
    {
        data_ = origin.proximityData();
        return *this;
    }

    bool operator==(const Proximity& right) const
    {
        ProximityData rdata = right.proximityData();
        return (data_.value_ == rdata.value_ &&
                data_.withinProximity_ == rdata.withinProximity_ &&
                data_.timestamp_ == rdata.timestamp_);
    }

private:
    ProximityData data_;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const Proximity& data);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Proximity& data);
};

Q_DECLARE_METATYPE( Proximity )

// Marshall the Proximity data into a D-Bus argument
inline QDBusArgument &operator<<(QDBusArgument &argument, const Proximity &data)
{
    argument.beginStructure();
    argument << data.data_.timestamp_ << data.data_.value_ << data.data_.withinProximity_;
    argument.endStructure();
    return argument;
}

// Retrieve the Proximity data from the D-Bus argument
inline const QDBusArgument &operator>>(const QDBusArgument &argument, Proximity &data)
{
    argument.beginStructure();
    argument >> data.data_.timestamp_ >> data.data_.value_ >> data.data_.withinProximity_;
    argument.endStructure();
    return argument;
}

#endif // PROXIMITY_H
