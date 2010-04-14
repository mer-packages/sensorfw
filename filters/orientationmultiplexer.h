#ifndef ORIENTATIONMULTIPLEXER_H
#define ORIENTATIONMULTIPLEXER_H

#include "sensord/filter.h"
#include "orientationdata.h"

class OrientationMultiplexer : public QObject, public FilterBase
{
    Q_OBJECT

public:
    OrientationMultiplexer();

private:
    void resetFilter();

    // input sinks (pads) of filter
    Sink<OrientationMultiplexer, AccelerationData>     accelerationSink_;
    Sink<OrientationMultiplexer, MagneticFieldData>    magneticFieldSink_;
    Sink<OrientationMultiplexer, AngularVelocityData>  angularVelocitySink_;

    // output sink (pad) of filter
    Source<AccMagGyrData>                       accMagGyrSource_;

    // buffers and availability flags for temporary storage of incoming data
    bool                                        accelerationAvailableFlag_;
    AccelerationData                            accelerationBuffer_;
    bool                                        magneticFieldAvailableFlag_;
    MagneticFieldData                           magneticFieldBuffer_;
    bool                                        angularVelocityAvailableFlag_;
    AngularVelocityData                         angularVelocityBuffer_;

    // methods that are called upon arrivial of input data (from buffers)
    void accelerationAvailable(unsigned n, const AccelerationData* values);
    void magneticFieldAvailable(unsigned n, const MagneticFieldData* values);
    void angularVelocityAvailable(unsigned n, const AngularVelocityData* values);

    // method that multiplexes the three separate output data types into a single output data type
    void outputter(const AccelerationData* acceleration,
                   const MagneticFieldData* magneticField,
                   const AngularVelocityData* angularVelocity);
};

#endif // ORIENTATIONMULTIPLEXER_H
