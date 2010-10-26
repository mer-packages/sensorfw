/**
   @file samplechainplugin.cpp
   @brief Sample plugin for chains

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

#include "samplechainplugin.h"
#include "samplechain.h"
#include "sensormanager.h"
#include <QtDebug>

void SampleChainPlugin::Register(class Loader&)
{
    qDebug() << "registering samplechain";
    SensorManager& sm = SensorManager::instance();
    sm.registerChain<SampleChain>("samplechain");
}

// Specify other required plugins in this function. Dependencies are
// anything that is required for this plugin to get data and filter it
// (adaptors, filters, other chains).
//
// Here we have one adaptor and one filter that we need.
QStringList SampleChainPlugin::Dependencies() {
    return QString("sampleadaptor:samplefilter").split(":", QString::SkipEmptyParts);
}

Q_EXPORT_PLUGIN2(samplechain, SampleChainPlugin)
