#include "motionbandadaptorplugin.h"
#include "motionbandadaptor.h"
#include "sensormanager.h"
#include <QtDebug>

void MotionBandAdaptorPlugin::Register(class Loader&)
{
    qDebug() << "registering motionbandadaptor";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<MotionBandAdaptor>("motionbandadaptor");
}

Q_EXPORT_PLUGIN2(motionbandadaptor, MotionBandAdaptorPlugin)
