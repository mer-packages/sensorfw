#ifndef HEADTRACKFILTER_H
#define HEADTRACKFILTER_H

#include "sensord/filter.h"
#include "libfilter/ge_filter.h"
#include "motionbanddata.h"
#include "orientationdata.h"

class HeadTrackFilter : public QObject, public Filter<AccMagGyrData, HeadTrackFilter, DeviceAxesData>
{
    Q_OBJECT

    Q_PROPERTY(double gain READ gain WRITE setGain)
    Q_PROPERTY(bool headCompensation READ headCompensation WRITE setHeadCompensation )

public:
    HeadTrackFilter();

    double gain() const { return filter_.gain; }
    void setGain(double gain) { filter_.gain = gain; }

    bool headCompensation() const { return headCompensation_; }
    void setHeadCompensation( bool headCompensation ) { headCompensation_ = headCompensation; }

    void resetOrientation();

private:
    // algorithm that determines the orientation output values
    void calculateOrientation(unsigned n, const AccMagGyrData* values);

    t_filterdata filter_;
    bool headCompensation_;
};

#endif // HEADTRACKFILTER_H
