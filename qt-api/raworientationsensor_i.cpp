#include "sensormanagerinterface.h"
#include "raworientationsensor_i.h"

RawOrientationSensorChannelInterface::RawOrientationSensorChannelInterface(const QString &path, int sessionId)
    : AbstractSensorChannelInterface(path, RawOrientationSensorChannelInterface::staticInterfaceName(), sessionId)
{
}

const RawOrientationSensorChannelInterface* RawOrientationSensorChannelInterface::listenInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    // TODO: can conversion from class name to type string be automated?
    if ( !sm.registeredAndCorrectClassName( id, RawOrientationSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<const RawOrientationSensorChannelInterface*>(sm.listenInterface(id));
}

RawOrientationSensorChannelInterface* RawOrientationSensorChannelInterface::controlInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, RawOrientationSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<RawOrientationSensorChannelInterface*>(sm.controlInterface(id));
}
