#include "oemtabletalsadaptor-asciiplugin.h"
#include "oemtabletalsadaptor-ascii.h"
#include "sensormanager.h"

void OEMTabletALSAdaptorAsciiPlugin::Register(class Loader&)
{
    sensordLogW() << "registering oemtabletalsadaptor-ascii";
    SensorManager& sm = SensorManager::instance();
    sm.registerDeviceAdaptor<OEMTabletALSAdaptorAscii>("alsadaptor");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(alsadaptor, OEMTabletALSAdaptorAsciiPlugin)
#endif
