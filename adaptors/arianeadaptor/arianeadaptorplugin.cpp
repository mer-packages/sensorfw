#include "arianeadaptorplugin.h"
#include "arianeadaptor.h"
#include "sensormanager.h"
#include <QtDebug>

void ArianeAdaptorPlugin::Register(class Loader&)
{
    qDebug() << "registering ArianeAdaptorPlugin";
}

Q_EXPORT_PLUGIN2(arianeadaptor, ArianeAdaptorPlugin)
