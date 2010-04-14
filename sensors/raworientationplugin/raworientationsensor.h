#ifndef RAW_ORIENTATION_CHANNEL_H
#define RAW_ORIENTATION_CHANNEL_H

#include "abstractsensor.h"
#include "raworientationsensor_a.h"
#include "sensord/dbusemitter.h"
#include "filters/motionbanddata.h"
#include "filters/headtrackfilter.h"
#include "datatypes/raworientation.h"
//#include "adaptors/mbadaptor/motionbandadaptor.h"

class Bin;
class MotionBandAdaptor;
template <class TYPE> class BufferReader;
class FilterBase;

class RawOrientationSensorChannel :
    public AbstractSensorChannel,
    public DbusEmitter<DeviceAxesData>
{
    Q_OBJECT

    Q_PROPERTY(bool headCompensation READ headCompensation WRITE setHeadCompensation )

public:
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        RawOrientationSensorChannel* sc = new RawOrientationSensorChannel(id);
        new RawOrientationSensorChannelAdaptor(sc);

        return sc;
    }

    /* TODO: filter properties could be propagated hooking into the dynamic property system:
     *       e.g. setProperty("filterName:propertyName", value)
     *       if there would be a mechanism to resolve them by name (not possible now)
     *       Furthermore there is the access control issue
     */
    bool headCompensation() const { return headTrackFilter_->headCompensation(); }
    void setHeadCompensation( bool headCompensation ) { headTrackFilter_->setHeadCompensation(headCompensation); }

public Q_SLOTS:
    void resetOrientation();
    void start();
    void stop();

signals:
    void dataAvailable(const RawOrientation& data);

protected:
    RawOrientationSensorChannel(const QString& id);
    virtual ~RawOrientationSensorChannel();

private:
    void emitToDbus(const DeviceAxesData& value);

    Bin*                            filterBin_;
    Bin*                            marshallingBin_;
    MotionBandAdaptor*              motionBand_;
    BufferReader<AccMagGyrData>*    accMagGyrReader_;
    HeadTrackFilter*                headTrackFilter_;
    RingBuffer<DeviceAxesData>*     outputBuffer_;
};

#endif
