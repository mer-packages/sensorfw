#include "steaccelerometeradaptorplugin.h"
#include "steaccelerometeradaptor.h"
#include "sensormanager.h"
#include "logging.h"

void SteAccelerometerAdaptorPlugin::Register(class Loader&)
{
    sensordLogD() << "registering steaccelerometeradaptor";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<SteAccelAdaptor>("accelerometeradaptor");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(steaccelerometeradaptor,SteAccelerometerAdaptorPlugin)
#endif
