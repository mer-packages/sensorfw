/**
   @file stabilityfilter.h
   @brief StabilityFilter
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

#ifndef STABILITYFILTER_H
#define STABILITYFILTER_H

#include "sensord/filter.h"

#include <ContextProvider>

#include <QPair>
#include <QTimer>

/*!

    \class StabilityFilter

    \brief Filter for providing the Orientation.IsStable context property.

    StabilityFilter computes the Orientation.IsStable property from a
    QPair<double, double> which contains the moving average and moving
    variance of the data. StabilityFilter pushes the data forward
    unchanged.

*/

using ContextProvider::Property;

class StabilityFilter : public QObject, public Filter<QPair<double, double>, StabilityFilter, QPair<double, double> >
{
    Q_OBJECT

public:
    StabilityFilter(Property* stableProperty, Property* unstableProperty,
                    double lowThreshold, double highThreshold, double hysteresis = 0.0);

public Q_SLOTS:
    void timeoutTriggered();

private:
    double lowThreshold;
    double highThreshold;
    double hysteresis;
    Property* stableProperty;
    Property* unstableProperty;
    void interpret(unsigned, const QPair<double, double>* data);
    QTimer timer;

    static const int timeout;
};

#endif
