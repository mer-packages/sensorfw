/**
   @file magnetometerscalefilter.h
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

#ifndef MAGNETOMETERSCALEFILTER_H
#define MAGNETOMETERSCALEFILTER_H

#include "orientationdata.h"
#include "sensord/filter.h"

/**
 * @brief Scaling filter for CalibratedMagneticFieldData.
 *
 * Performs linear scaling for all three axis values. Scale factor is
 * applied to all three axis values.
 */
class MagnetometerScaleFilter : public QObject, public Filter<CalibratedMagneticFieldData, MagnetometerScaleFilter, CalibratedMagneticFieldData>
{
    Q_OBJECT;

public:

    /**
     * Factory method.
     * @return New filter instance as FilterBase*.
     */
    static FilterBase* factoryMethod() {
        return new MagnetometerScaleFilter;
    }

protected:

    MagnetometerScaleFilter();

private:
    void filter(unsigned, const CalibratedMagneticFieldData*);

    int factor;
};

#endif
