#ifndef ARIANE_SENSOR_I_H
#define ARIANE_SENSOR_I_H

#include <QtDBus/QtDBus>

#include "abstractsensor_i.h"
#include "serviceinfo.h"
#include "xyz.h"
#include "unsigned.h"
#include "key.h"

/*
 * Proxy class for interface local.ArianeSensor
 */
class ArianeSensorChannelInterface: public AbstractSensorChannelInterface
{
    Q_OBJECT

public:
    static inline const char *staticInterfaceName()
    { return "local.ArianeSensor"; }

    static QDBusAbstractInterface* factoryMethod(const QString& id, int sessionId)
    {
        // TODO: see which arguments can be made explicit
        return new ArianeSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
    }

public:
    ArianeSensorChannelInterface(const QString &path, int sessionId);

    Q_PROPERTY(int hardwareSensorId READ hardwareSensorId WRITE setHardwareSensorId)
    int hardwareSensorId() { return qvariant_cast< int >(internalPropGet("hardwareSensorId")); }
    void setHardwareSensorId(int hwSensorId) { internalPropSet("hardwareSensorId", qVariantFromValue(hwSensorId)); }

    Q_PROPERTY(QString adaptor READ adaptor WRITE setAdaptor)
    QString adaptor() const { return qvariant_cast< QString >(internalPropGet("adaptor")); }
    void setAdaptor(const QString& adaptor) { internalPropSet("adaptor", qVariantFromValue(adaptor)); }

    virtual ~ArianeSensorChannelInterface() {}

    static const ArianeSensorChannelInterface* listenInterface(const QString& id);
    static ArianeSensorChannelInterface* controlInterface(const QString& id);

public Q_SLOTS: // METHODS
    inline QDBusReply<bool> setAdaptorProperty ( const QString& name, unsigned value )
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(name) << qVariantFromValue(value);
        return callWithArgumentList(QDBus::Block, QLatin1String("setAdaptorProperty"), argumentList);
    }

    inline QDBusReply<unsigned> adaptorProperty ( const QString& name ) const
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(name);
        return const_cast<ArianeSensorChannelInterface*>(this)->callWithArgumentList(QDBus::Block, QLatin1String("adaptorProperty"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void dataAvailable(const XYZ& data);
    void dataAvailable(const Unsigned& data);
    void dataAvailable(const Key& data);
};

namespace local {
  typedef ::ArianeSensorChannelInterface ArianeSensor;
}

#endif
