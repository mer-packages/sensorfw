#include "sensormanagerinterface.h"
#include "arianesensor_i.h"

ArianeSensorChannelInterface::ArianeSensorChannelInterface(const QString &path, int sessionId)
    : AbstractSensorChannelInterface(path, ArianeSensorChannelInterface::staticInterfaceName(), sessionId)
{
}

const ArianeSensorChannelInterface* ArianeSensorChannelInterface::listenInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    // TODO: can conversion from class name to type string be automated?
    if ( !sm.registeredAndCorrectClassName( id, ArianeSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<const ArianeSensorChannelInterface*>(sm.listenInterface(id));
}

ArianeSensorChannelInterface* ArianeSensorChannelInterface::controlInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, ArianeSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<ArianeSensorChannelInterface*>(sm.controlInterface(id));
}
