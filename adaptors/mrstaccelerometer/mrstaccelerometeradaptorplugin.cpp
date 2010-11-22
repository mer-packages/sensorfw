#include "mrstaccelerometeradaptorplugin.h"
#include "mrstaccelerometeradaptor.h"
#include "sensormanager.h"
#include "logging.h"

void MRSTAccelerometerAdaptorPlugin::Register(class Loader&)
{
    sensordLogD() << "registering mrstaccelerometeradaptor";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<MRSTAccelAdaptor>("accelerometeradaptor");
}

Q_EXPORT_PLUGIN2(accelerometeradaptor-mrst, MRSTAccelerometerAdaptorPlugin)
