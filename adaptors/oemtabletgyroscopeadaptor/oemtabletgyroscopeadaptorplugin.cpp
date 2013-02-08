#include "oemtabletgyroscopeadaptorplugin.h"
#include "oemtabletgyroscopeadaptor.h"
#include "sensormanager.h"

void OEMTabletGyroscopeAdaptorPlugin::Register(class Loader&)
{
    sensordLogD() << "registering oemtabletgyroscopeadaptor";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<OEMTabletGyroscopeAdaptor>("gyroscopeadaptor");
}

Q_EXPORT_PLUGIN2(gyroscopeadaptor, OEMTabletGyroscopeAdaptorPlugin)
