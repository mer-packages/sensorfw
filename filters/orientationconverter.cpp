#include "orientationconverter.h"

OrientationConverter::OrientationConverter()
  : Filter<TimedXyzData, OrientationConverter, OrientationData>(this, &OrientationConverter::converter)
{
}

void OrientationConverter::converter(unsigned n, const TimedXyzData* xyzData)
{
    for (unsigned i = 0; i < n; i++)
    {
        OrientationData orientation;

        // do trivial conversion here...
        orientation.x_ = xyzData[i].x_;
        orientation.y_ = xyzData[i].y_;
        orientation.z_ = xyzData[i].z_;

        source_.propagate(1, &orientation);
    }
}
