/**
   @file sensormanager.h
   @brief SensorManager

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include "abstractsensor.h"
#include "abstractchain.h"
#include "deviceadaptor.h"
#include "filter.h"
#include "sfwerror.h"
#include "idutils.h"
#include "parameterparser.h"
#include "logging.h"

#ifdef SENSORFW_MCE_WATCHER
#include "mcewatcher.h"
#else
class MceWatcher;
#endif

class QSocketNotifier;
class SocketHandler;

/**
 * Sensor instance entry. Contains list of connected sessions.
 */
class SensorInstanceEntry
{
public:
    /**
     * Constructor.
     *
     * @param type sensor type.
     */
    SensorInstanceEntry(const QString& type);

    /**
     * Destructor.
     */
    ~SensorInstanceEntry();

    QSet<int>               sessions_; /**< connected sessions. */
    AbstractSensorChannel*  sensor_;   /**< sensor channel */
    QString                 type_;     /**< type */
};

/**
 * Filter chain instance.
 */
class ChainInstanceEntry
{
public:
    /**
     * Constructor.
     *
     * @param type chain type.
     */
    ChainInstanceEntry(const QString& type);

    /**
     * Destructor.
     */
    ~ChainInstanceEntry();

    int                     cnt_;   /**< Reference count */
    AbstractChain*          chain_; /**< Chain pointer  */
    QString                 type_;  /**< Type */
};

/**
 * Adaptor instance.
 */
class DeviceAdaptorInstanceEntry
{
public:
    /**
     * Constructor.
     *
     * @param type Adaptor type.
     * @param id Adaptor ID.
     */
    DeviceAdaptorInstanceEntry(const QString& type, const QString& id);

    /**
     * Destructor.
     */
    ~DeviceAdaptorInstanceEntry();

    QMap<QString, QString>  propertyMap_; /**< Property map */
    DeviceAdaptor*          adaptor_;     /**< Adaptor pointer */
    int                     cnt_;         /**< Reference count */
    QString                 type_;        /**< Type */
};

/**
 * Sensor manager. Singleton class which manages client sessions and
 * track existence and usage of sensors, chains and adaptors.
 */
class SensorManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SensorManager)
    Q_PROPERTY(SensorManagerError errorCode READ errorCode)
    Q_PROPERTY(int errorCodeInt READ errorCodeInt)
    Q_PROPERTY(QString errorString READ errorString)

public:
    /**
     * Append current status into given StringList.
     *
     * @param output StringList to append status.
     */
    void printStatus(QStringList& output) const;

    /**
     * Get last occured error code.
     *
     * @return error code.
     */
    SensorManagerError errorCode() const;

    /**
     * Get last occured error code.
     *
     * @return error code.
     */
    int errorCodeInt() const;

    /**
     * Get last occured error description.
     *
     * @return error description.
     */
    const QString& errorString() const;

    /**
     * Get singleton instance.
     *
     * @return sensor manager singleton.
     */
    static SensorManager& instance();

    /**
     * Register DBus service.
     *
     * @return was service registered succesfully.
     */
    bool registerService();

    /**
     * Register given sensor type.
     *
     * @tparam SENSOR_TYPE sensor class.
     * @param sensorName sensor name
     */
    template<class SENSOR_TYPE>
    void registerSensor(const QString& sensorName);

    /**
     * Register given chain type.
     *
     * @tparam CHAIN_TYPE chain class.
     * @param chainName chain name
     */
    template<class CHAIN_TYPE>
    void registerChain(const QString& chainName);

    /**
     * Request chain.
     *
     * @param id chain ID.
     */
    AbstractChain* requestChain(const QString& id);

    /**
     * Release chain.
     *
     * @param id chain ID.
     */
    void releaseChain(const QString& id);

    /**
     * Register given adaptor type.
     *
     * @tparam DEVICE_ADAPTOR_TYPE adaptor class.
     * @param adaptorName adaptor name
     */
    template<class DEVICE_ADAPTOR_TYPE>
    void registerDeviceAdaptor(const QString& adaptorName);

    /**
     * Request adaptor.
     *
     * @param id adaptor ID.
     */
    DeviceAdaptor* requestDeviceAdaptor(const QString& id);

    /**
     * Release adaptor.
     *
     * @param id adaptor ID.
     */
    void releaseDeviceAdaptor(const QString& id);

    /**
     * Register given filter type.
     *
     * @tparam FILTER_TYPE filter class.
     * @param filterName filter name
     */
    template<class FILTER_TYPE>
    void registerFilter(const QString& filterName);

    /**
     * Provides a new instance of the requested filter.
     *
     * @param id filter ID.
     * @return Pointer to instance of the requested filter. The instance must be
     *         deleted when not needed anymore. Filter instances are never shared
     *         between filter chains.
     */
    FilterBase* instantiateFilter(const QString& id);

#ifdef SM_PRINT
    /**
     * Print sensor manager status to logger.
     */
    void print() const; // for debugging purposes only
#endif

    /**
     * Write sensor data for given session.
     *
     * @param id Session ID.
     * @param source Source from where to write.
     * @param size How many bytes to write.
     */
    bool write(int id, const void* source, int size);

    /**
     * Load plugin.
     *
     * @param name plugin name.
     * @return was plugin loaded succesfully.
     */
    bool loadPlugin(const QString& name);

    /**
     * Request sensor.
     *
     * @param id Sensor ID.
     * @return new session ID for the sensor.
     */
    int requestSensor(const QString& id);

    /**
     * Release sensor.
     *
     * @param id Sensor ID.
     * @param sessionId Session ID.
     */
    bool releaseSensor(const QString& id, int sessionId);

    /**
     * Get sensor instance.
     *
     * @param id Sensor ID.
     * @return sensor instance pointer or NULL if not found.
     */
    const SensorInstanceEntry* getSensorInstance(const QString& id) const;

    /**
     * Get socket handler.
     *
     * @return socket handler.
     */
    SocketHandler& socketHandler() const;

    /**
     * Get list configured of adaptor types.
     */
    QList<QString> getAdaptorTypes() const;

    /**
     * Get list configured of adaptor types.
     */
    int getAdaptorCount(const QString& type) const;

#ifdef SENSORFW_MCE_WATCHER
    /**
     * Get pointer to MceWatcher instance.
     *
     * @return MceWatcher instance pointer.
     */
    MceWatcher* MCEWatcher() const;
#endif

    double magneticDeviation();
    void setMagneticDeviation(double level);

private Q_SLOTS:
    /**
     * Callback for lost session connections.
     *
     * @param sessionId Session ID.
     */
    void lostClient(int sessionId);

    /**
     * Callback for MCE display state change event.
     *
     * @param displayState display state.
     */
    void displayStateChanged(bool displayState);

    /**
     * Callback for MCE powersave mode change event.
     *
     * @param deviceMode device PSM state.
     */
    void devicePSMStateChanged(bool deviceMode);

    /**
     * Callback for arrived sensor data in internal pipe which SensorManager
     * needs to propagate to the SocketHandler.
     */
    void sensorDataHandler(int);

Q_SIGNALS:
    /**
     * Signal for occured errors.
     *
     * @param error error code.
     */
    void errorSignal(int error);

    /**
     * Signal to resume background calibration.
     */
    void resumeCalibration();

    /**
     * Signal to stop background calibration.
     */
    void stopCalibration();

    /**
     * Signal that display is turned on.
     */
    void displayOn();

private:
    /**
     * Constructor.
     */
    SensorManager();

    /**
     * Destructor.
     */
    virtual ~SensorManager();

    /**
     * Set error state.
     *
     * @param errorCode error code.
     * @param errorString error description.
     */
    void setError(SensorManagerError errorCode, const QString& errorString);

    /**
     * Clear error state.
     */
    void clearError();

    /**
     * Add sensor with given ID.
     *
     * @param id sensor ID.
     */
    AbstractSensorChannel* addSensor(const QString& id);

    /**
     * Remove sensor with given ID.
     *
     * @param id sensor ID.
     */
    void removeSensor(const QString& id);

    /**
     * Generate new unique session ID.
     *
     * @return session ID.
     */
    int createNewSessionId();

    /**
     * Resolve peer PID of given session.
     *
     * @param id session ID.
     * @return PID string.
     */
    QString socketToPid(int id) const;

    /**
     * Resolve peer PIDs of given sessions.
     *
     * @param ids Session IDs.
     * @return PID string.
     */
    QString socketToPid(const QSet<int>& ids) const;

    QMap<QString, SensorChannelFactoryMethod>      sensorFactoryMap_; /**< factories for sensor types */
    QMap<QString, SensorInstanceEntry>             sensorInstanceMap_; /**< sensor instances */

    QMap<QString, DeviceAdaptorFactoryMethod>      deviceAdaptorFactoryMap_; /**< factories for adaptor types. */
    QMap<QString, DeviceAdaptorInstanceEntry>      deviceAdaptorInstanceMap_; /**< adaptor instances */

    QMap<QString, ChainFactoryMethod>              chainFactoryMap_; /**< factories for chain types. */
    QMap<QString, ChainInstanceEntry>              chainInstanceMap_; /**< chain instances */

    QMap<QString, FilterFactoryMethod>             filterFactoryMap_; /**< factories for filter types */

    SocketHandler*                                 socketHandler_; /**< socket handler */
    MceWatcher*                                    mceWatcher_; /**< MCE watcher */
    SensorManagerError                             errorCode_; /** global error code */
    QString                                        errorString_; /** global error description */
    int                                            pipefds_[2]; /** pipe for sensor samples */
    QSocketNotifier*                               pipeNotifier_; /** notifier for pipe stream */

    static SensorManager*                          instance_; /** singleton */
    static int                                     sessionIdCount_; /** session ID counter */

    double deviation;
};

template<class SENSOR_TYPE>
void SensorManager::registerSensor(const QString& sensorName)
{
    if (sensorInstanceMap_.contains(sensorName)) {
        sensordLogW() << QString("<%1> Sensor is already present!").arg(sensorName);
        return;
    }

    QString typeName = SENSOR_TYPE::staticMetaObject.className();
    sensorInstanceMap_.insert(sensorName, SensorInstanceEntry(typeName));

    if ( !sensorFactoryMap_.contains(typeName) )
    {
        sensorFactoryMap_[typeName] = SENSOR_TYPE::factoryMethod;
    }

    if (sensorFactoryMap_[typeName] != SENSOR_TYPE::factoryMethod) {
        sensordLogW() << "Sensor type doesn't match!";
        return;
    }
}

template<class CHAIN_TYPE>
void SensorManager::registerChain(const QString& chainName)
{
    if (chainInstanceMap_.contains(chainName)) {
        sensordLogW() << QString("<%1> Chain is already present!").arg(chainName);
        return;
    }

    QString typeName = CHAIN_TYPE::staticMetaObject.className();
    chainInstanceMap_.insert(chainName, ChainInstanceEntry(typeName));

    if (!chainFactoryMap_.contains(typeName))
    {
        chainFactoryMap_[typeName] = CHAIN_TYPE::factoryMethod;
    }

    if (chainFactoryMap_[typeName] != CHAIN_TYPE::factoryMethod) {
        sensordLogW() << "Chain type doesn't match!";
        return;
    }
}

template<class DEVICE_ADAPTOR_TYPE>
void SensorManager::registerDeviceAdaptor(const QString& adaptorName)
{
    QString cleanAdaptorName = getCleanId(adaptorName);

    if (deviceAdaptorInstanceMap_.contains(cleanAdaptorName)) {
        sensordLogW() << QString("<%1> Adaptor is already present!").arg(cleanAdaptorName);
        return;
    }

    QString typeName = DEVICE_ADAPTOR_TYPE::staticMetaObject.className();
    deviceAdaptorInstanceMap_.insert(cleanAdaptorName, DeviceAdaptorInstanceEntry(typeName, adaptorName));

    if ( !deviceAdaptorFactoryMap_.contains(typeName) )
    {
        deviceAdaptorFactoryMap_[typeName] = DEVICE_ADAPTOR_TYPE::factoryMethod;
    }

    if (deviceAdaptorFactoryMap_[typeName] != DEVICE_ADAPTOR_TYPE::factoryMethod) {
        sensordLogW() << "Device adaptor type doesn't match!";
        return;
    }
}

template<class FILTER_TYPE>
void SensorManager::registerFilter(const QString& filterName)
{
    if (filterFactoryMap_.contains(filterName)) {
        sensordLogW() << QString("<%1> Filter is already present!").arg(filterName);
        return;
    }

    filterFactoryMap_[filterName] = FILTER_TYPE::factoryMethod;
}

#endif
