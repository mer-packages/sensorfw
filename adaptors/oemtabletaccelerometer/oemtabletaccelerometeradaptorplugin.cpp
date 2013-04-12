#include "oemtabletaccelerometeradaptorplugin.h"
#include "oemtabletaccelerometeradaptor.h"
#include "sensormanager.h"
#include "logging.h"

void OemtabletAccelerometerAdaptorPlugin::Register(class Loader&)
{
    sensordLogD() << "registering oemtabletaccelerometeradaptor";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<OemtabletAccelAdaptor>("accelerometeradaptor");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(oemtabletaccelerometeradaptor, OemtabletAccelerometerAdaptorPlugin)
#endif
