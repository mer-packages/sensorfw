#include "raworientationsensor_a.h"

RawOrientationSensorChannelAdaptor::RawOrientationSensorChannelAdaptor(QObject* parent) :
    AbstractSensorChannelAdaptor(parent)
{
    setAutoRelaySignals(true);
}

void RawOrientationSensorChannelAdaptor::resetOrientation()
{
    // handle method call local.RawOrientationSensor.resetOrientation
    QMetaObject::invokeMethod(parent(), "resetOrientation");
}

void RawOrientationSensorChannelAdaptor::setHeadCompensation( bool headCompensation )
{
    // set the value of property headCompensation
    parent()->setProperty("headCompensation", headCompensation);
}

bool RawOrientationSensorChannelAdaptor::headCompensation() const
{
    return qvariant_cast<int>(parent()->property("headCompensation"));
}
