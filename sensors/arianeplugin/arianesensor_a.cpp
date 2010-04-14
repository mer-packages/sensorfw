#include "arianesensor_a.h"

ArianeSensorChannelAdaptor::ArianeSensorChannelAdaptor(QObject* parent) :
    AbstractSensorChannelAdaptor(parent)
{
    setAutoRelaySignals(true);
}

int ArianeSensorChannelAdaptor::hardwareSensorId() const
{
    return qvariant_cast< int >(parent()->property("hardwareSensorId"));
}

void ArianeSensorChannelAdaptor::setHardwareSensorId(int hwSensorId)
{
    parent()->setProperty("hardwareSensorId", hwSensorId);
}

QString ArianeSensorChannelAdaptor::adaptor() const
{
    return qvariant_cast< QString >(parent()->property("adaptor"));
}

void ArianeSensorChannelAdaptor::setAdaptor(const QString& adaptor)
{
    parent()->setProperty("adaptor", adaptor);
}

bool ArianeSensorChannelAdaptor::setAdaptorProperty ( const QString& name, unsigned value )
{
    // handle method call local.ArianeSensor.setAdaptorProperty
    bool out0;
    QArgument<QString> arg("const QString&", name);
    QMetaObject::invokeMethod(parent(), "setAdaptorProperty", Q_RETURN_ARG(bool, out0), arg, Q_ARG(unsigned, value));
    return out0;
}

unsigned ArianeSensorChannelAdaptor::adaptorProperty ( const QString& name ) const
{
    // handle method call local.ArianeSensor.adaptorProperty
    unsigned out0;
    QArgument<QString> arg("const QString&", name);
    QMetaObject::invokeMethod(parent(), "adaptorProperty", Q_RETURN_ARG(unsigned, out0), arg);
    return out0;
}

