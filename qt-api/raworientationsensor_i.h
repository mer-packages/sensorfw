#ifndef HEADSENSOR_I_H
#define HEADSENSOR_I_H

#include <QtDBus/QtDBus>

#include "abstractsensor_i.h"
#include "raworientation.h"
#include "serviceinfo.h"

/*
 * Proxy class for interface local.HeadOrientationSensor
 */
class RawOrientationSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT

public:
    static inline const char *staticInterfaceName()
    { return "local.RawOrientationSensor"; }

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId)
    {
        // ToDo: see which arguments can be made explicit
        return new RawOrientationSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
    }

    RawOrientationSensorChannelInterface(const QString &path, int sessionId);

    Q_PROPERTY(bool headCompensation READ headCompensation WRITE setHeadCompensation )
    inline bool headCompensation() const { return qvariant_cast< int >(internalPropGet("headCompensation")); }
    inline void setHeadCompensation( bool headCompensation ) { internalPropSet("headCompensation", qVariantFromValue(headCompensation)); }

    // made these virtual by hand...
    virtual ~RawOrientationSensorChannelInterface() {}

    static const RawOrientationSensorChannelInterface* listenInterface(const QString& id);
    static RawOrientationSensorChannelInterface* controlInterface(const QString& id);

public Q_SLOTS: // METHODS
    inline QDBusReply<void> resetOrientation()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("resetOrientation"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void dataAvailable(const RawOrientation& data);
};

namespace local {
  typedef ::RawOrientationSensorChannelInterface HeadOrientationSensor;
}

#endif
