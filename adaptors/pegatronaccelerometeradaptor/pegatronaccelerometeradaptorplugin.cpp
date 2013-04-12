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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(pegatronaccelerometeradaptor, PegatronAccelerometerAdaptorPlugin)
#endif
