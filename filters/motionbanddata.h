#ifndef MOTIONBANDDATA_H
#define MOTIONBANDDATA_H

#include "libmb/ge_types.h"

struct AccMagGyrData
{
    t_vector acc; // Accelerometer
    t_vector mag; // Magnetometer
    t_vector gyr; // Gyroscope
};

class DeviceAxesData
{
public:
    DeviceAxesData() {}
    DeviceAxesData(const t_vector& xValue, const t_vector& yValue, const t_vector& zValue)
        : x(xValue), y(yValue), z(zValue) {}

    t_vector x; // x axis of the device in reference coordinate system
    t_vector y; // y axis of the device in reference coordinate system
    t_vector z; // z axis of the device in reference coordinate system
};

#endif // MOTIONBANDDATA_H
