#ifndef FILTERED_SENSOR_CHANNEL_H
#define FILTERED_SENSOR_CHANNEL_H

#include "abstractsensor.h"
#include "arianeadaptor/arianeadaptor.h"
#include "sensord/dbusemitter.h"
#include "datatypes/orientation.h"

class Bin;
class DeviceAdaptor;
template <class TYPE> class BufferReader;
class FilterBase;

class FilteredSensorChannel :
    public AbstractSensorChannel,
    public DbusEmitter<OrientationData>
{
    Q_OBJECT

    Q_PROPERTY(int interval READ interval WRITE setInterval)
    Q_PROPERTY(QString adaptor READ adaptor WRITE setAdaptor)

public:
    static AbstractSensorChannel* factoryMethod(const QString& id);

    virtual int  interval();
    virtual void setInterval(int interval);

    QString adaptor() const { return adaptor_; }
    void setAdaptor(const QString& adaptor) { adaptor_ = adaptor; }

    // TODO: this is better implemented using some sort of scoping, e.g. arianeadaptor::hwSensorId=10
    bool setAdaptorProperty ( const QString& name, unsigned value );
    unsigned adaptorProperty ( const QString& name );

public Q_SLOTS:
    virtual bool start();
    virtual bool stop();

Q_SIGNALS:
    void dataAvailable(const Orientation& data);

protected:
    static const QString ORIENTATION_HW_SENSOR_NAME;

    FilteredSensorChannel(const QString& id);
    virtual ~FilteredSensorChannel();

private:
    bool checkPreconditionValid();

    void emitToDbus(const OrientationData& value);

    Bin*                            filterBin_;
    Bin*                            marshallingBin_;

    ArianeAdaptor*                  ariane_;
    QString                         adaptor_;

    BufferReader<OrientationData>*  orientationReader_;
    FilterBase*                     orientationConvertor_;
    RingBuffer<OrientationData>*    outputBuffer_;
};

#endif
