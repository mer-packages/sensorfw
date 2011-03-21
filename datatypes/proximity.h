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

/**
 * QObject facade for #ProximityData.
 */
class Proximity : public Unsigned
{
    Q_OBJECT

    Q_PROPERTY(int reflectance READ reflectance)
    Q_PROPERTY(int withinProximity READ withinProximity)

public:

    /**
     * Default constructor
     */
    Proximity() {}

    /**
     * Constructor
     *
     * @param data Source object.
     */
    Proximity(const ProximityData& data) :
        Unsigned(TimedUnsigned(data.timestamp_, data.withinProximity_)),
        data_(data.timestamp_, data.value_, data.withinProximity_)
        {}

    /**
     * Copy constructor
     *
     * @param data Source object.
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
     * Accessor for proximity status.
     *
     * @return is an object in proximity of sensor or not
     */
    bool withinProximity() const { return data_.withinProximity_; }

    /**
     * Accessor for raw reflectance reading.
     *
     * @return proximity reflectance reading.
     */
    int reflectance() const { return data_.value_; }

    /**
     * Assignment operator.
     *
     * @param origin Source object for assigment.
     */
    Proximity& operator=(const Proximity& origin)
    {
        data_ = origin.proximityData();
        return *this;
    }

    /**
     * Comparison operator.
     *
     * @param right Object to compare to.
     * @return comparison result.
     */
    bool operator==(const Proximity& right) const
    {
        ProximityData rdata = right.proximityData();
        return (data_.value_ == rdata.value_ &&
                data_.withinProximity_ == rdata.withinProximity_ &&
                data_.timestamp_ == rdata.timestamp_);
    }

private:
    ProximityData data_; /**< Contained proximity reading. */

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Proximity& data);
};

Q_DECLARE_METATYPE( Proximity )

/**
 * Marshall the Proximity data into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param data data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const Proximity &data)
{
    argument.beginStructure();
    argument << data.proximityData().timestamp_ << data.proximityData().value_ << data.proximityData().withinProximity_;
    argument.endStructure();
    return argument;
}

/**
 * Unmarshall Proximity data from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param data unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, Proximity &data)
{
    argument.beginStructure();
    argument >> data.data_.timestamp_ >> data.data_.value_ >> data.data_.withinProximity_;
    argument.endStructure();
    return argument;
}

#endif // PROXIMITY_H
