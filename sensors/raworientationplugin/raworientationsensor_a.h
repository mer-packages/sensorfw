#ifndef HEAD_ORIENTATION_SENSOR_CHANNEL_A_H
#define HEAD_ORIENTATION_SENSOR_CHANNEL_A_H

#include <QtDBus/QtDBus>

#include "filters/motionbanddata.h"
#include "datatypes/raworientation.h"
#include "abstractsensor_a.h"

class RawOrientationSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.RawOrientationSensor")

public:
    Q_PROPERTY(bool headCompensation READ headCompensation WRITE setHeadCompensation )
    bool headCompensation() const;
    void setHeadCompensation( bool headCompensation );

    RawOrientationSensorChannelAdaptor(QObject* parent);

public Q_SLOTS:
    void resetOrientation();

Q_SIGNALS:
    void dataAvailable(const RawOrientation& data);
};

#endif
