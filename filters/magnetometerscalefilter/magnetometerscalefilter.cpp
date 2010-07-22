/**
   @file magnetometerscalefilter.cpp
   @brief MagnetometerScaleFilter

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

#include "magnetometerscalefilter.h"
#include "sensord/config.h"

MagnetometerScaleFilter::MagnetometerScaleFilter() :
        Filter<CalibratedMagneticFieldData, MagnetometerScaleFilter, CalibratedMagneticFieldData>(this, &MagnetometerScaleFilter::filter)
{
}

void MagnetometerScaleFilter::filter(unsigned, const CalibratedMagneticFieldData* data)
{
    int factor = Config::configuration()->value("magnetometer_scale_coefficient", QVariant(300)).toInt();;
    CalibratedMagneticFieldData transformed;

    transformed.timestamp_ = data->timestamp_;
    transformed.level_ = data->level_;
    transformed.x_ = data->x_ * factor;
    transformed.y_ = data->y_ * factor;
    transformed.z_ = data->z_ * factor;
    transformed.rx_ = data->rx_ * factor;
    transformed.ry_ = data->ry_ * factor;
    transformed.rz_ = data->rz_ * factor;

    source_.propagate(1, &transformed);
}
