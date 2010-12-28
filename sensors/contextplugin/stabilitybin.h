/**
   @file stabilitybin.h
   @brief Stability Bin for ContextFW
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>
   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#ifndef STABILITY_BIN
#define STABILITY_BIN

#include "sensord/bin.h"
#include "sensord/bufferreader.h"
#include "sensord/abstractchain.h"
#include "datatypes/orientationdata.h"

#include "normalizerfilter.h"
#include "cutterfilter.h"
#include "avgvarfilter.h"
#include "stabilityfilter.h"

#include <ContextProvider>

#include <QPair>

class DeviceAdaptor;

class StabilityBin : public QObject, Bin
{
    Q_OBJECT

public:
    StabilityBin(ContextProvider::Service& service);
    ~StabilityBin();

private Q_SLOTS:
    void startRun();
    void stopRun();

private:
    ContextProvider::Property isStableProperty;
    ContextProvider::Property isShakyProperty;
    ContextProvider::Group group;

    BufferReader<AccelerationData> accelerometerReader;
    DeviceAdaptor* accelerometerAdaptor;

    NormalizerFilter normalizerFilter;
    CutterFilter cutterFilter;
    AvgVarFilter avgVarFilter;
    StabilityFilter stabilityFilter;

    int sessionId;

    static const int STABILITY_THRESHOLD;
    static const int UNSTABILITY_THRESHOLD;
    static const float STABILITY_HYSTERESIS;
};



#endif
