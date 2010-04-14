#ifndef ARIANE_SENSOR_CHANNEL_A_H
#define ARIANE_SENSOR_CHANNEL_A_H

#include <QtDBus/QtDBus>

#include "xyz.h"
#include "unsigned.h"
#include "key.h"

#include "abstractsensor_a.h"

class ArianeSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.ArianeSensor")

public:
    ArianeSensorChannelAdaptor(QObject* parent);

    Q_PROPERTY(int hardwareSensorId READ hardwareSensorId WRITE setHardwareSensorId)
    int hardwareSensorId() const;
    void setHardwareSensorId(int hwSensorId);

    Q_PROPERTY(QString adaptor READ adaptor WRITE setAdaptor)
    QString adaptor() const;
    void setAdaptor(const QString& adaptor);

public Q_SLOTS: // METHODS
    bool setAdaptorProperty ( const QString& name, unsigned value );
    unsigned adaptorProperty ( const QString& name ) const;

Q_SIGNALS:
    void dataAvailable(const XYZ& data);
    void dataAvailable(const Unsigned& data);
    void dataAvailable(const Key& data);
};

#endif
