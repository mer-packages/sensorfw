#include <QtDebug>

#include "orientationmultiplexer.h"

OrientationMultiplexer::OrientationMultiplexer() :
    accelerationSink_(this,     &OrientationMultiplexer::angularVelocityAvailable),
    magneticFieldSink_(this,    &OrientationMultiplexer::magneticFieldAvailable),
    angularVelocitySink_(this,  &OrientationMultiplexer::angularVelocityAvailable),
    accelerationAvailableFlag_(false),
    accelerationBuffer_(0, 0, 0, 0),
    magneticFieldAvailableFlag_(false),
    magneticFieldBuffer_(0, 0, 0, 0),
    angularVelocityAvailableFlag_(false),
    angularVelocityBuffer_(0, 0, 0, 0)
{
    addSink(&accelerationSink_,     "acceleration");
    addSink(&magneticFieldSink_,    "magneticfield");
    addSink(&angularVelocitySink_,  "angularvelocity");

    addSource(&accMagGyrSource_,    "source");
}

void OrientationMultiplexer::accelerationAvailable(unsigned n, const AccelerationData* values)
{
    Q_ASSERT(n == 1);

    if (magneticFieldAvailableFlag_ && angularVelocityAvailableFlag_) {
        outputter(values, &magneticFieldBuffer_, &angularVelocityBuffer_);
    } else {
        accelerationBuffer_        = *values;
        accelerationAvailableFlag_ = true;
    }
}

void OrientationMultiplexer::magneticFieldAvailable(unsigned n, const MagneticFieldData* values)
{
    Q_ASSERT(n == 1);

    if (accelerationAvailableFlag_ && angularVelocityAvailableFlag_) {
        outputter(&accelerationBuffer_, values, &angularVelocityBuffer_);
    } else {
        magneticFieldBuffer_        = *values;
        magneticFieldAvailableFlag_ = true;
    }
}

void OrientationMultiplexer::angularVelocityAvailable(unsigned n, const AngularVelocityData* values)
{
    Q_ASSERT(n == 1);

    if (accelerationAvailableFlag_ && magneticFieldAvailableFlag_) {
        outputter(&accelerationBuffer_, &magneticFieldBuffer_, values);
    } else {
        angularVelocityBuffer_        = *values;
        angularVelocityAvailableFlag_ = true;
    }
}

void OrientationMultiplexer::outputter(const AccelerationData* acceleration,
                                       const MagneticFieldData* magneticField,
                                       const AngularVelocityData* angularVelocity)
{
    qDebug() << __PRETTY_FUNCTION__;

    AccMagGyrData accMagGyr;

    // TODO: there is a lot of copying (type conversion going on here)
    accMagGyr.acc.x = acceleration->x_;
    accMagGyr.acc.y = acceleration->y_;
    accMagGyr.acc.z = acceleration->z_;

    accMagGyr.mag.x = magneticField->x_;
    accMagGyr.mag.y = magneticField->y_;
    accMagGyr.mag.z = magneticField->z_;

    accMagGyr.gyr.x = angularVelocity->x_;
    accMagGyr.gyr.y = angularVelocity->y_;
    accMagGyr.gyr.z = angularVelocity->z_;

    accMagGyrSource_.propagate(1, &accMagGyr);

    accelerationAvailableFlag_    = false;
    magneticFieldAvailableFlag_   = false;
    angularVelocityAvailableFlag_ = false;
}
