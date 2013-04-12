#ifndef OEMTABLETMAGNETOMETERADAPTOR_PLUGIN_H
#define OEMTABLETMAGNETOMETERADAPTOR_PLUGIN_H

#include "plugin.h"

class OemtabletMagnetometerAdaptorPlugin : public Plugin
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "com.nokia.SensorService.Plugin/1.0" FILE "plugin.json")
#endif
private:
    void Register(class Loader& l);
};

#endif
