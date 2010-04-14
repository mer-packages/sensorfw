#ifndef FILTERED_SENSOR_CHANNEL_A_H
#define FILTERED_SENSOR_CHANNEL_A_H

#include <QtDBus/QtDBus>

#include "datatypes/orientation.h"
#include "abstractsensor_a.h"

class FilteredSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.FilteredSensor")

public:
    FilteredSensorChannelAdaptor(QObject* parent);

Q_SIGNALS:
    void dataAvailable(const Orientation& data);
};

#endif
