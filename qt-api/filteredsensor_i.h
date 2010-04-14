#ifndef FILTEREDSENSOR_I_H
#define FILTEREDSENSOR_I_H

#include <QtDBus/QtDBus>

#include "abstractsensor_i.h"
#include "orientation.h"
#include "serviceinfo.h"

/*
 * Proxy class for interface local.Sensor
 */
class FilteredSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT

public:
    static inline const char *staticInterfaceName()
    { return "local.FilteredSensor"; }

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId)
    {
        // ToDo: see which arguments can be made explicit
        return new FilteredSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
    }

public:
    FilteredSensorChannelInterface(const QString &path, int sessionId);

    // made these virtual by hand...
    virtual ~FilteredSensorChannelInterface() {}

    static const FilteredSensorChannelInterface* listenInterface(const QString& id);
    static FilteredSensorChannelInterface* controlInterface(const QString& id);

public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
    void dataAvailable(const Orientation& data);
};

namespace local {
  typedef ::FilteredSensorChannelInterface FilteredSensor;
}

#endif
