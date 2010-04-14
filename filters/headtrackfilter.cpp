#include "libfilter/orientationconversion.h"

#include "headtrackfilter.h"

HeadTrackFilter::HeadTrackFilter()
  : Filter<AccMagGyrData, HeadTrackFilter, DeviceAxesData>(this, &HeadTrackFilter::calculateOrientation)
{
    ge_filter_reset(&filter_);
    filter_.gain = DEFAULT_GAIN;
}

void HeadTrackFilter::resetOrientation()
{
    ge_filter_calibrate_north(&filter_);

    // Compensate for the non-standard orientation of the sensor in the headphones...
    if ( headCompensation_ )
    {
        qDebug() << "compensating";
        t_vector tmp;
        tmp = filter_.caly;
        filter_.caly = filter_.calz;
        ge_vset(&filter_.calz, -tmp.x, -tmp.y, -tmp.z);
    }
    else
    {
        qDebug() << "NO compensating";
    }
}

void HeadTrackFilter::calculateOrientation(unsigned n, const AccMagGyrData* amgData)
{
    for (unsigned i = 0; i < n; i++)
    {
        OrientationData orientation;

        // do conversion here...
        ge_gyrfilter(&amgData[i].acc, &amgData[i].mag, &amgData[i].gyr, &filter_);

        DeviceAxesData axesData;

        axesData.x = filter_.x;
        axesData.y = filter_.y;
        axesData.z = filter_.z;

        source_.propagate(1, &axesData);
    }
}
