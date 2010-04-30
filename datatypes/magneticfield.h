/**
   @file magneticfield.h
   @brief QObject based datatype for CalibratedMagneticFieldData

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Tapio Rantala <ext-tapio.rantala@nokia.com>

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

#ifndef MAGNETICFIELDDATA_H
#define MAGNETICFIELDDATA_H

#include <QtDebug>
#include <QDBusArgument>

#include <filters/orientationdata.h>

/**
*/
class MagneticField : public QObject //AbstractSensorData
{
public:
    Q_OBJECT;

public:

    /**
     * Default constructor (needed for Qt metatype system)
     */
    MagneticField() : QObject() {}

    /**
     * Copy constructor (needed for Qt metatype system)
     */
    MagneticField(const CalibratedMagneticFieldData& calibratedData) : QObject() {
        data_.timestamp_ = calibratedData.timestamp_;
        data_.level_ = calibratedData.level_;
        data_.x_ = calibratedData.x_;
        data_.y_ = calibratedData.y_;
        data_.z_ = calibratedData.z_;
        data_.rx_ = calibratedData.rx_;
        data_.ry_ = calibratedData.ry_;
        data_.rz_ = calibratedData.rz_; 
    }

    /**
     * Copy constructor (needed for Qt metatype system)
     */
    MagneticField(const MagneticField& data) : QObject() {
        data_.timestamp_ = data.data_.timestamp_;
        data_.level_ = data.data_.level_;
        data_.x_ = data.data_.x_;
        data_.y_ = data.data_.y_;
        data_.z_ = data.data_.z_;
        data_.rx_ = data.data_.rx_;
        data_.ry_ = data.data_.ry_;
        data_.rz_ = data.data_.rz_; 
    }

    /**
     */
    const CalibratedMagneticFieldData& data() const { return data_; }

    MagneticField& operator=(const MagneticField& origin)
    {
        data_.timestamp_ = origin.data_.timestamp_;
        data_.level_ = origin.data_.level_;
        data_.x_ = origin.data_.x_;
        data_.y_ = origin.data_.y_;
        data_.z_ = origin.data_.z_;
        data_.rx_ = origin.data_.rx_;
        data_.ry_ = origin.data_.ry_;
        data_.rz_ = origin.data_.rz_; 

        return *this;
    }

    bool operator==(const MagneticField& right) const
    {
        CalibratedMagneticFieldData rdata = right.data();
        return (data_.x_ == rdata.x_ &&
                data_.y_ == rdata.y_ &&
                data_.z_ == rdata.z_ &&
                data_.rx_ == rdata.rx_ &&
                data_.ry_ == rdata.ry_ &&
                data_.rz_ == rdata.rz_ &&
                data_.level_ == rdata.level_ &&
                data_.timestamp_ == rdata.timestamp_);
    }

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

     /**
     * Returns the raw value for X.
     * @return raw x value.
     */
    int rx() const { return data_.rx_; }

    /**
     * Returns the raw value for Y.
     * @return raw y value.
     */
    int ry() const { return data_.ry_; }

    /**
     * Returns the raw value for Z.
     * @return raw z value.
     */
    int rz() const { return data_.rz_; }

    /**
     * Returns the magnetometer calibration level.
     * @return level of magnetometer calibration.
     */
    int level() const { return data_.level_; }

    /**
     * Returns the timestamp.
     * @return timestamp value.
     */
    quint64 timestamp() const { return data_.timestamp_; }

private:
    CalibratedMagneticFieldData data_;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const MagneticField& data);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, MagneticField& data);
};

Q_DECLARE_METATYPE( MagneticField )


inline
QDBusArgument &operator<<(QDBusArgument &argument, const MagneticField &data)
{
    argument.beginStructure();
    argument << data.data_.timestamp_ << data.data_.level_;
    argument << data.data_.x_ << data.data_.y_ << data.data_.z_;
    argument << data.data_.rx_ << data.data_.ry_ << data.data_.rz_;
    argument.endStructure();
    return argument;
}

inline
const QDBusArgument &operator>>(const QDBusArgument &argument, MagneticField &data)
{
    argument.beginStructure();
    argument >> data.data_.timestamp_ >> data.data_.level_;
    argument >> data.data_.x_ >> data.data_.y_ >> data.data_.z_;
    argument >> data.data_.rx_ >> data.data_.ry_ >> data.data_.rz_;
    argument.endStructure();
    return argument;
}

#endif // MAGNETICFIELDDATA_H
