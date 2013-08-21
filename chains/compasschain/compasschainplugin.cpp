/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com

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
 */

#include "compasschainplugin.h"
#include "compasschain.h"
#include "compassfilter.h"
#include "sensormanager.h"
#include "logging.h"

void CompassChainPlugin::Register(class Loader&)
{
    sensordLogD() << "registering compasschain";
    SensorManager& sm = SensorManager::instance();

    sm.registerChain<CompassChain>("compasschain");
    sm.registerFilter<CompassFilter>("compassfilter");
}

QStringList CompassChainPlugin::Dependencies() {
//    return QString("accelerometerchain:magcalibrationchain:declinationfilter:downsamplefilter:avgaccfilterr").split(":", QString::SkipEmptyParts);
    return QString("accelerometerchain:magcalibrationchain:declinationfilter:downsamplefilter:avgaccfilter:orientationadaptor").split(":", QString::SkipEmptyParts);
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(compasschain, CompassChainPlugin)
#endif
