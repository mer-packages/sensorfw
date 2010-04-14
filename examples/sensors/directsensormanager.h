#ifndef DIRECTSENSORMANAGER_H
#define DIRECTSENSORMANAGER_H

#include "idutils.h"

#include "sensormanager.h"

template<class SENSOR_TYPE>
class DirectSensorFactory
{
public:
    static SENSOR_TYPE* controlSensor(const QString& id)
    {
        SensorManager& sm = SensorManager::instance();

        SENSOR_TYPE* sensor = NULL;
        int sessionId = sm.requestControlSensor(id);
        if ( sessionId != INVALID_SESSION )
        {
            sensor = dynamic_cast<SENSOR_TYPE*>(sm.getSensorInstance(getCleanId(id)).sensor_);
            if ( sensor )
            {
                sessionMap_.insert(sensor, sessionId);
            }
            else
            {
                qDebug() << __PRETTY_FUNCTION__ << "obtained sensor of different type, dynamic cast failed";
            }
        }

        return sensor;
    }

    static const SENSOR_TYPE* listenSensor(const QString& id)
    {
        SensorManager& sm = SensorManager::instance();

        const SENSOR_TYPE* sensor = NULL;
        int sessionId = sm.requestListenSensor(id);
        if ( sessionId != INVALID_SESSION )
        {
            sensor = dynamic_cast<SENSOR_TYPE*>(sm.getSensorInstance(getCleanId(id)).sensor_);
            if ( sensor )
            {
                sessionMap_.insert(sensor, sessionId);
            }
            else
            {
                qDebug() << __PRETTY_FUNCTION__ << "obtained sensor of different type, dynamic cast failed";
            }
        }

        return sensor;
    }

    static bool releaseSensor(SENSOR_TYPE* sensor)
    {
        SensorManager& sm = SensorManager::instance();

        Q_ASSERT( sensor && sessionMap_.contains(sensor) );
        return sm.releaseSensor(sensor->id(), sessionMap_[sensor]);
    }

private:
    static QMap<AbstractSensorChannel*, int> sessionMap_;
};

template<class SENSOR_TYPE>
QMap<AbstractSensorChannel*, int> DirectSensorFactory<SENSOR_TYPE>::sessionMap_;

#endif // DIRECTSENSORMANAGER_H
