#include "orientationoffsetter.h"

// TODO: support for situation where multiple packets arrive in single call

void OrientationOffsetter::orientationAvailable(unsigned               n,
                                                const OrientationData* values)
{
    Q_ASSERT(n == 1);

    if (offsetAvailable_) {
        filter(values, &offsetBuffer_);
    } else {
        orientationBuffer_    = *values;
        orientationAvailable_ = true;
    }
}

void OrientationOffsetter::offsetAvailable(unsigned        n,
                                           const unsigned* values)
{
    Q_ASSERT(n == 1);

    if (orientationAvailable_) {
        filter(&orientationBuffer_, values);
    } else {
        offsetBuffer_    = *values;
        offsetAvailable_ = true;
    }
}

void OrientationOffsetter::filter(const OrientationData* orientation,
                                  const unsigned*        offset)
{
    OrientationData output = *orientation;
    output.x_ += *offset;
    output.y_ += *offset;
    output.z_ += *offset;

    source_.propagate(1, &output);

    orientationAvailable_ = false;
    offsetAvailable_      = false;
}
