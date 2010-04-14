#include "raworientationplugin.h"
#include "raworientationsensor.h"
#include "sensormanager.h"
#include <QtDebug>

void RawOrientationPlugin::Register(class Loader&)
{
    qDebug() << "registering raworientationsensor";
    SensorManager& sm = SensorManager::instance();
    sm.registerSensor<RawOrientationSensorChannel>("raworientationsensor");
}

QStringList RawOrientationPlugin::Dependencies() {
    return QStringList("motionbandadaptor");
}

Q_EXPORT_PLUGIN2(raworientationsensor, RawOrientationPlugin)
