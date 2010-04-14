#include "arianeplugin.h"
#include "arianesensor.h"
#include "sensormanager.h"
#include <QtDebug>

void ArianePlugin::Register(class Loader&)
{
    qDebug() << "registering ArianePlugin";
}

QStringList ArianePlugin::Dependencies() {
    return QStringList("arianeadaptor");
}

Q_EXPORT_PLUGIN2(arianesensor, ArianePlugin)
