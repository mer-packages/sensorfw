/**
   @file avgvarfilter.cpp
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

#include "avgvarfilter.h"
#include <math.h>

AvgVarFilter::AvgVarFilter(int size) :
    Filter<double, AvgVarFilter, QPair<double, double> >(this, &AvgVarFilter::interpret),
    size(size), samplesReceived(0), current(0), samples(size), samplesSquared(size), sampleSum(0), sampleSquareSum(0)

{
}

void AvgVarFilter::interpret(unsigned, const double* data)
{
    // Ramp-up-phase:
    if (samplesReceived < size) {
        samples[samplesReceived] = *data;
        samplesSquared[samplesReceived] = (*data)*(*data);
        sampleSum += *data;
        sampleSquareSum += (*data)*(*data);
        ++samplesReceived;
        return;
    }

    //qDebug() << "Data received on AvgVarFilter:" << *data;
    //qDebug() << "Cur data:" << samples;

    // Moving average & variance computations:
    // Remove the oldest sample, replace with the new sample
    sampleSum = sampleSum - samples[current] + *data;
    sampleSquareSum = sampleSquareSum - samples[current] * samples[current] + (*data) * (*data);

    // Take the new value in
    samples[current] = *data;
    ++current;
    if (current >= size) {
        current = 0;
    }

    double avg = sampleSum / size;
    double var = (size * sampleSquareSum - (sampleSum * sampleSum)) / (size * (size - 1));

    //qDebug() << "Avg and var" << avg << var;

    QPair<double, double> pair(avg, var);
    source_.propagate(1, &pair);
}

// Start the ramp-up again
void AvgVarFilter::reset()
{
    samplesReceived = 0;
    current = 0;
    sampleSum = 0;
    sampleSquareSum = 0;
}
