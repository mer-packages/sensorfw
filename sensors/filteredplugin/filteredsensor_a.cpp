#include "filteredsensor_a.h"

FilteredSensorChannelAdaptor::FilteredSensorChannelAdaptor(QObject* parent) :
    AbstractSensorChannelAdaptor(parent)
{
    setAutoRelaySignals(true);
}
