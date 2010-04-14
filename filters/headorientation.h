#ifndef HEADORIENTATION_H
#define HEADORIENTATION_H

#include "sensord/filter.h"
#include "orientationdata.h"
#include "ge_filter.h"
#include "motionbanddata.h"

class HeadOrientation : public QObject, public FilterBase
{
    Q_OBJECT

    Q_PROPERTY(double gain READ gain WRITE setGain)

public:
    HeadOrientation();

    double gain() const { return filterContext_.gain; }
    void setGain(double gain) { filterContext_.gain = gain; }

private:
    void resetFilter();

    // input sink (pad) of filter
    Sink<HeadOrientation, AccMagGyrData> sink_; // input pad

    // output sink (pad) of filter
    Source<DeviceAxesData>               source_; // output pad

    // algorithm that determines the output values
    void filter(unsigned n, const AccMagGyrData* values);

    t_filterdata filterContext_;
};

#endif // HEADORIENTATION_H
