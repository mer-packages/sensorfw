#ifndef ARIANEENTRY_H
#define ARIANEENTRY_H

#include <QMap>

#include "sensord/deviceadaptor.h"
#include "sfwerror.h"

#include "ariane/ariane.h"

/*
  * ARIANE_DEVICE_LOCATION_WRIST is a bad name
  * When is ariane_sensor_get_parameter_ids needed?
*/

class ArianeAdaptor;

class ArianeAdaptedSensorEntry : public AdaptedSensorEntry {
    Q_OBJECT

    Q_PROPERTY(SensorError errorCode READ errorCode)

    // overloaded (because they affect subsystem parameter / properties)
    Q_PROPERTY(unsigned interval READ interval WRITE setInterval)

    Q_PROPERTY( uint16_t hwSensorId READ hwSensorId);
    Q_PROPERTY( uint8_t  measurementType READ measurementType);
    Q_PROPERTY( uint8_t  sensorLocation READ sensorLocation);
    Q_PROPERTY( uint8_t  sensorNumber READ sensorNumber);
    Q_PROPERTY( uint16_t sampleRate READ sampleRate);
    Q_PROPERTY( uint8_t  resolutionBits READ resolutionBits);
    Q_PROPERTY( uint8_t  sampleChannels READ sampleChannels);
    Q_PROPERTY( uint16_t flags READ flags);
    // TODO conversion is not exposed as property: da_unit_conversion_t conversion[DA_MAX_SAMPLE_VALUES];

public:
    ArianeAdaptedSensorEntry(ArianeAdaptor& adaptor,
                             ariane_session_id_t hwSensorId,
                             const QString& description,
                             const ariane_sensor_properties_t* sensorProps);
    virtual ~ArianeAdaptedSensorEntry();

    // note: sensor subsystem properties do not change, hence no requests to sensor subsystem necessary
    uint16_t hwSensorId() const      { return hwSensorId_; }
    uint8_t  measurementType() const { return ARIANE_SID_GET_MTYPE(hwSensorId_); }
    uint8_t  sensorLocation() const  { return ARIANE_SID_GET_LOCATION(hwSensorId_); }
    uint8_t  sensorNumber() const    { return ARIANE_SID_GET_NUM(hwSensorId_); }
    uint16_t sampleRate() const      { Q_ASSERT( sensorProps_ ); return sensorProps_->sample_rate; }
    uint8_t  resolutionBits() const  { Q_ASSERT( sensorProps_ ); return sensorProps_->resolution_bits; }
    uint8_t  sampleChannels() const  { Q_ASSERT( sensorProps_ ); return sensorProps_->sample_channels; }
    uint16_t flags() const           { Q_ASSERT( sensorProps_ ); return sensorProps_->flags; }

    // these are overloaded (because they affect subsystem parameter / properties)
    virtual unsigned interval();
    virtual void setInterval(unsigned interval);

    SensorError errorCode() const { return errorCode_; }

    // these are overloaded (because they affect subsystem parameter / properties)
    bool setProperty ( const char * name, const QVariant & value );
    QVariant property ( const char * name ) const;

    void print() const;

    QMap<QString, ariane_sensor_parameter_t> paramMap_;

signals:
    void propertyChanged(const QString& name) const;

protected:
    static const char* ARIANE_PARAM_NAME_SAMPLING_RATE;

    bool subsystemParameter( const char * paramName, uint32_t& newValue );
    bool setSubsystemParameter( const char * paramName, uint32_t value );

    virtual void eventHandler(const ariane_event_t *event);
    void eventHandlerVariant(const ariane_event_t *event);

    void createRingBuffer();
    void destroyRingBuffer();

private:
    static void printEvent(const ariane_event_t *event, void* cookie);
    static void staticEventHandler(const ariane_event_t *event, void* cookie);

    ariane_session_id_t hwSensorId_;

    ariane_sensor_t* sensor_;

    // parameters & properties provided by Ariane subsystem HW (these are read only properties)
    ariane_sensor_properties_t* sensorProps_;

    // handle to parent adaptor for updating/proxying sensor subsystem properties and parameters
    ArianeAdaptor& parentAdaptor_;

    SensorError errorCode_;

    friend class ArianeAdaptor;
};

#endif // ARIANEENTRY_H
