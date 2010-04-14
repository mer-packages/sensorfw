#ifndef ORIENTATIONOFFSETTER_H
#define ORIENTATIONOFFSETTER_H

#include "sensord/filter.h"
#include "orientationdata.h"

class OrientationOffsetter : public FilterBase
{
public:
    OrientationOffsetter() :
        orientation_(this, &OrientationOffsetter::orientationAvailable),
        offset_(this,      &OrientationOffsetter::offsetAvailable),
        orientationAvailable_(false),
        offsetAvailable_(false),
        orientationBuffer_(0, 0, 0, 0),
        offsetBuffer_(0)
    {
        addSink(&orientation_, "orientation");
        addSink(&offset_,      "offset");
        addSource(&source_,    "source");
    }

private:
    Sink<OrientationOffsetter, OrientationData> orientation_;
    Sink<OrientationOffsetter, unsigned>        offset_;
    Source<OrientationData>                     source_;
    bool                                        orientationAvailable_;
    bool                                        offsetAvailable_;
    OrientationData                             orientationBuffer_;
    unsigned                                    offsetBuffer_;

    void orientationAvailable(unsigned n, const OrientationData* values);
    void offsetAvailable(unsigned n, const unsigned* values);
    void filter(const OrientationData* orientation,
                const unsigned*        offset);
};

#endif
