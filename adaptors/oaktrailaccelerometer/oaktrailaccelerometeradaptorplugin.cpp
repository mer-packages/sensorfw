#include "oaktrailaccelerometeradaptorplugin.h"
#include "oaktrailaccelerometeradaptor.h"
#include "sensormanager.h"
#include "logging.h"

void OaktrailAccelerometerAdaptorPlugin::Register(class Loader&)
{
    sensordLogD() << "registering oaktrailaccelerometeradaptor";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<OaktrailAccelAdaptor>("accelerometeradaptor");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(oaktrailaccelerometeradaptor, OaktrailAccelerometerAdaptorPlugin)
#endif
