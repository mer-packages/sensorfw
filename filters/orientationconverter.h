#ifndef ORIENTATIONCONVERTER_H
#define ORIENTATIONCONVERTER_H

#include "sensord/filter.h"
#include "orientationdata.h"

class OrientationConverter : public QObject, public Filter<TimedXyzData, OrientationConverter, OrientationData>
{
public:
    OrientationConverter();

private:
    // algorithm that determines the output values
    void converter(unsigned n, const TimedXyzData* values);
};

#endif // ORIENTATIONCONVERTER_H
