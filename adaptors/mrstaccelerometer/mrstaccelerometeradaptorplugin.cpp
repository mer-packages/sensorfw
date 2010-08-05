#include "mrstaccelerometeradaptorplugin.h"
#include "mrstaccelerometeradaptor.h"
#include "sensormanager.h"
#include <QtDebug>

void MRSTAccelerometerAdaptorPlugin::Register(class Loader&)
{
    qDebug() << "registering mrstaccelerometeradaptor";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<MRSTAccelAdaptor>("accelerometeradaptor");
}

Q_EXPORT_PLUGIN2(accelerometeradaptor-mrst, MRSTAccelerometerAdaptorPlugin)
