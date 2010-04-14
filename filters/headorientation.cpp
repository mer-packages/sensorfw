#include <QtDebug>

#include "headorientation.h"

HeadOrientation::HeadOrientation()
  : sink_(this, &HeadOrientation::filter)
{
    addSink(&sink_, "input");
    addSource(&source_, "output");

    // TODO: add control that enables resetting / restarting of the filter...
    resetFilter();
}

void HeadOrientation::resetFilter()
{
    ge_filter_reset(&filterContext_);
}

void HeadOrientation::filter(unsigned n, const AccMagGyrData* values)
{
    qDebug() << __PRETTY_FUNCTION__;

    for ( int i = 0; i < n; i++ )
    {
        DeviceAxesData deviceAxes;

        // filtering for orientation: conver acc, mag, gyt into device orientation axes
        ge_gyrfilter(&values[i].acc, &values[i].mag, &values[i].gyr, &filterContext_);

        // TODO: there is a lot of copying (type conversion) going on here
        deviceAxes.x = filterContext_.x;
        deviceAxes.y = filterContext_.y;
        deviceAxes.z = filterContext_.z;

        print_filter_data(&filterContext_);

        source_.propagate(1, &deviceAxes);
    }
}
