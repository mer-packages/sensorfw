#ifndef ARIANESENSOR_H
#define ARIANESENSOR_H

#include "abstractsensor.h"
#include "arianeadaptor/arianeadaptor.h"
#include "sensord/bufferreader.h"
#include "sensord/dbusemitter.h"
#include "datatypes/xyz.h"
#include "datatypes/key.h"
#include "datatypes/unsigned.h"

class Bin;
class ArianeAdaptor;

class ArianeSensorChannel :
    public AbstractSensorChannel,
    public DbusEmitter<QVariant>
{
    Q_OBJECT

    Q_PROPERTY(int interval READ interval WRITE setInterval)

    Q_PROPERTY(int hardwareSensorId READ hardwareSensorId WRITE setHardwareSensorId)
    Q_PROPERTY(QString adaptor READ adaptor WRITE setAdaptor)

public:
    static AbstractSensorChannel* factoryMethod(const QString& id);

    virtual int  interval();
    virtual void setInterval(int interval);

    int hardwareSensorId() const { return hwSensorId_; }
    void setHardwareSensorId(int hwSensorId) { hwSensorId_ = hwSensorId; }

    QString adaptor() const { return adaptor_; }
    void setAdaptor(const QString& adaptor) { adaptor_ = adaptor; }

public Q_SLOTS:
    // TODO: this is better implemented using some sort of scoping, e.g. arianeadaptor::hwSensorId=10
    bool setAdaptorProperty ( const QString& name, unsigned value );
    unsigned adaptorProperty ( const QString& name );

    virtual bool start();
    virtual bool stop();

Q_SIGNALS:
    void dataAvailable(const XYZ& data);
    void dataAvailable(const Unsigned& data);
    void dataAvailable(const Key& data);

protected:
    ArianeSensorChannel(const QString& id);
    virtual ~ArianeSensorChannel();

private:
    bool checkPreconditionValid();

    void emitToDbus(const QVariant& value);

    int             hwSensorId_;

    Bin*            marshallingBin_;
    ArianeAdaptor*  ariane_;
    QString         adaptor_;
};

#endif // ARIANESENSOR_H
