/**
   @file orientationbin.h
   @brief Orientation Bin for ContextFW
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

#ifndef ORIENTATION_BIN_H
#define ORIENTATION_BIN_H

#include "sensord/bin.h"
#include "sensord/bufferreader.h"
#include "accelerometeradaptor.h"
#include "filters/orientationdata.h"

#include "normalizerfilter.h"
#include "cutterfilter.h"
#include "avgvarfilter.h"
#include "stabilityfilter.h"
#include "screeninterpreterfilter.h"

#include <ContextProvider>

#include <QPair>

class DeviceAdaptor;

class OrientationBin : public QObject, Bin
{
    Q_OBJECT

public:
    OrientationBin(ContextProvider::Service& service);
    ~OrientationBin();

private Q_SLOTS:
    void startRun();
    void stopRun();

private:
    ContextProvider::Property topEdgeProperty;
    ContextProvider::Property isCoveredProperty;
    ContextProvider::Property isStableProperty;
    ContextProvider::Property isShakyProperty;
    ContextProvider::Group group;

    BufferReader<AccelerationData> accelerometerReader;
    DeviceAdaptor* accelerometerAdaptor;

    ScreenInterpreterFilter screenInterpreterFilter;
    NormalizerFilter normalizerFilter;
    CutterFilter cutterFilter;
    AvgVarFilter avgVarFilter;
    StabilityFilter stabilityFilter;
};

#endif
