#include "filteredplugin.h"
#include "filteredsensor.h"
#include "sensormanager.h"
#include <QtDebug>

void FilteredPlugin::Register(class Loader&)
{
    qDebug() << "registering FilteredPlugin";
}

QStringList FilteredPlugin::Dependencies() {
    return QStringList("arianeadaptor");
}

Q_EXPORT_PLUGIN2(filteredsensor, FilteredPlugin)
