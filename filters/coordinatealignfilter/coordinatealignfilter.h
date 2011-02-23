/**
   @file coordinatealignfilter.h
   @brief CoordinateAlignFilter

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#ifndef COORDINATEALIGNFILTER_H
#define COORDINATEALIGNFILTER_H

#include "datatypes/orientationdata.h"
#include "filter.h"

/**
 * TMatrix holds a transformation matrix.
 */
class TMatrix {
private:
    static const int DIM = 3;

public:
    TMatrix() {
        setMatrix((const double[DIM][DIM]){{1,0,0},{0,1,0},{0,0,1}});
    }
    TMatrix(const TMatrix& other) {
        setMatrix(other.data_);
    }
    TMatrix(double m[][DIM]) {
        setMatrix(m);
    }

    double get(int i, int j) const {
        if (i >= DIM || j >= DIM || i < 0 || j < 0) {
            qWarning("Index out of bounds");
            return 0;
        }
        return data_[i][j];
    };

    void setMatrix(const double m[DIM][DIM]) {
        memcpy(data_, m, sizeof(double[DIM][DIM]));
    }

    double data_[DIM][DIM];
};
Q_DECLARE_METATYPE(TMatrix);

/**
 * @brief Coordinate alignment filter.
 *
 * Performs three dimensional coordinate transformations.
 * Transformation is described by transformation matrix which is set through
 * \c TMatrix property. Matrix must be of size 3x3. Default TMatrix is
 * identity matrix.
 */
class CoordinateAlignFilter : public QObject, public Filter<TimedXyzData, CoordinateAlignFilter, TimedXyzData>
{
    Q_OBJECT;
    Q_PROPERTY(TMatrix transMatrix READ matrix WRITE setMatrix);
public:

    /**
     * Factory method.
     * @return New CoordinateAlignFilter instance as FilterBase*.
     */
    static FilterBase* factoryMethod() {
        return new CoordinateAlignFilter;
    }

    const TMatrix& matrix() const { return matrix_; }

    void setMatrix(const TMatrix& matrix) { matrix_ = matrix; }

protected:
    /**
     * Constructor.
     */
    CoordinateAlignFilter();

private:
    void filter(unsigned, const TimedXyzData*);

    TMatrix matrix_;
};

#endif // COORDINATEALIGNFILTER_H
