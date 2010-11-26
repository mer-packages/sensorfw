/**
   @file compassbin.h
   @brief Compass Bin for ContextFW
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

#ifndef COMPASS_BIN_H
#define COMPASS_BIN_H

#include "sensord/bin.h"
#include "sensord/bufferreader.h"
#include "datatypes/orientationdata.h"

#include "headingfilter.h"

#include <ContextProvider>

class AbstractChain;

class CompassBin : public QObject, Bin
{
    Q_OBJECT

public:
    CompassBin(ContextProvider::Service& service, bool pluginValid = true);
    ~CompassBin();

private Q_SLOTS:
    void startRun();
    void stopRun();

private:
    Property headingProperty;

    AbstractChain* compassChain;
    BufferReader<CompassData> compassReader;
    HeadingFilter headingFilter;
};

#endif
