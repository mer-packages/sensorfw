#include "pegatronaccelerometeradaptorplugin.h"
#include "pegatronaccelerometeradaptor.h"
#include "sensormanager.h"
#include "logging.h"

void PegatronAccelerometerAdaptorPlugin::Register(class Loader&)
{
    sensordLogD() << "registering pegatronaccelerometeradaptor";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<PegatronAccelerometerAdaptor>("accelerometeradaptor");
}

Q_EXPORT_PLUGIN2(pegatronaccelerometeradaptor, PegatronAccelerometerAdaptorPlugin)
