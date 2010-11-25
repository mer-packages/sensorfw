/**
   @file avgvarfilter.h
   @brief Average variance filter
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>

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

#ifndef AVGVARFILTER_H
#define AVGVARFILTER_H

#include "sensord/filter.h"

#include <QPair>
#include <QVector>
#include <QMutex>

class AvgVarFilter : public QObject, public Filter<double, AvgVarFilter, QPair<double, double> >
{
    Q_OBJECT

public:
    AvgVarFilter(int samples);
    void reset();

private:
    int size;
    int samplesReceived;
    int current;
    QVector<double> samples;
    QVector<double> samplesSquared;
    double sampleSum;
    double sampleSquareSum;
    QMutex mutex;

    void interpret(unsigned, const double* data);
};

#endif
