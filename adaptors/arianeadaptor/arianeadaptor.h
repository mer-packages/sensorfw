#ifndef ARIANE_ADAPTOR_H
#define ARIANE_ADAPTOR_H

#include <QtDebug>
#include <QVariant>

#include "sensord/deviceadaptorringbuffer.h"
#include "filters/orientationdata.h"

#include "arianeentry.h"

/*
 *  TODO: Deal with DeviceAdaptors that may temporarily go out of range (e.g. Bluetooth)
 *  TODO: What kind of access control do we want for DeviceAdaptors? Same as Sensor (Subsystem)?
 */

class ArianeAdaptor : public DeviceAdaptor
{
    Q_OBJECT

    Q_PROPERTY(bool isRunning READ isRunning)
    Q_PROPERTY(bool variantMode READ variantMode WRITE setVariantMode)
    Q_PROPERTY(QString bluetoothAddress READ bluetoothAddress WRITE setBluetoothAddress)

public:
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new ArianeAdaptor(id);
    }

    virtual ~ArianeAdaptor();

    bool isRunning() const { return isRunning_; }
    bool variantMode() const { return variantMode_; }
    void setVariantMode(bool variantMode) { variantMode_ = variantMode; }

    QString bluetoothAddress() const { return btAddress_; }
    void setBluetoothAddress(const QString& bluetoothAddress) { btAddress_ = bluetoothAddress; }

    bool startAdaptor();
    void stopAdaptor();

    bool startSensor(const QString& sensorId);
    void stopSensor(const QString& sensorId);

public:
    static void printSubsystemErrorStr(int rc);

protected:
    ArianeAdaptor(const QString& id);

    virtual void handleTimedXyzData(RingBufferBase* ringBuffer, uint32_t timestamp, ariane_event_xyz16_t xyz);
    virtual void handleTimedUnsigned(RingBufferBase* ringBuffer, uint32_t timestamp, unsigned data);
    virtual void handleTimedKey(RingBufferBase* ringBuffer, uint32_t timestamp, ariane_event_key_t key);

    virtual void handleTimedXyzDataVariant(RingBufferBase* ringBuffer, uint32_t timestamp, ariane_event_xyz16_t xyz);
    virtual void handleTimedUnsignedVariant(RingBufferBase* ringBuffer, uint32_t timestamp, unsigned data);
    virtual void handleTimedKeyVariant(RingBufferBase* ringBuffer, uint32_t timestamp, ariane_event_key_t key);

private:
    void populateRegistry();
    void retrieveSensorInformation(ariane_sid_t sensor);

    static bool         arianeLibInitialized_;

    bool                isRunning_;
    bool                variantMode_;

    ariane_sensordev_t* dev_;
    QString             btAddress_;

    friend class ArianeAdaptedSensorEntry;
};

#endif // ARIANE_ADAPTOR_H
