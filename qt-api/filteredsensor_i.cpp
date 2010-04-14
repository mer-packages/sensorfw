#include "sensormanagerinterface.h"
#include "filteredsensor_i.h"

FilteredSensorChannelInterface::FilteredSensorChannelInterface(const QString &path, int sessionId)
    : AbstractSensorChannelInterface(path, FilteredSensorChannelInterface::staticInterfaceName(), sessionId)
{
}

const FilteredSensorChannelInterface* FilteredSensorChannelInterface::listenInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    // TODO: can conversion from class name to type string be automated?
    if ( !sm.registeredAndCorrectClassName( id, FilteredSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<const FilteredSensorChannelInterface*>(sm.listenInterface(id));
}

FilteredSensorChannelInterface* FilteredSensorChannelInterface::controlInterface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, FilteredSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<FilteredSensorChannelInterface*>(sm.controlInterface(id));
}
