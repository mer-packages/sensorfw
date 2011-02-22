/**
   @file sensormanager.h
   @brief SensorManager

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

class QSocketNotifier;
class SocketHandler;
class MceWatcher;

class SensorInstanceEntry {
public:
    SensorInstanceEntry(const QString& type = "none") : sensor_(0), type_(type) {}
    ~SensorInstanceEntry() {}

    QList<int>              sessions_;
    AbstractSensorChannel*  sensor_;
    QString                 type_;
};

class ChainInstanceEntry {
public:
    ChainInstanceEntry(const QString& type = "none") : cnt_(0), chain_(0), type_(type) {}
    ~ChainInstanceEntry() {}

    int                     cnt_;
    AbstractChain*          chain_;
    QString                 type_;
};

class DeviceAdaptorInstanceEntry {
public:
    DeviceAdaptorInstanceEntry(const QString& type = "none", const QString& id = "unknown") : adaptor_(0), cnt_(0), type_(type)
    { propertyMap_ = ParameterParser::getPropertyMap(id); }
    ~DeviceAdaptorInstanceEntry() {}

    QMap<QString, QString>  propertyMap_;
    DeviceAdaptor*          adaptor_;
    int                     cnt_;
    QString                 type_;
};

class SensorManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(SensorManagerError errorCode READ errorCode)
    Q_PROPERTY(int errorCodeInt READ errorCodeInt)
    Q_PROPERTY(QString errorString READ errorString)

public:
    void printStatus(QStringList& output) const;

    SensorManagerError errorCode() const { return errorCode_; }
    int errorCodeInt() const { return static_cast<int>(errorCode_); }
    QString errorString() const { return errorString_; }

    static SensorManager& instance();
    bool registerService();
    bool getPSMState();

    template<class SENSOR_TYPE>
    void registerSensor(const QString& sensorName);

    template<class CHAIN_TYPE>
    void registerChain(const QString& chainName);

    AbstractChain* requestChain(const QString& id);
    void releaseChain(const QString& id);

    template<class DEVICE_ADAPTOR_TYPE>
    void registerDeviceAdaptor(const QString& adaptorName);

    DeviceAdaptor* requestDeviceAdaptor(const QString& id);
    void releaseDeviceAdaptor(const QString& id);

    template<class FILTER_TYPE>
    void registerFilter(const QString& filterName);

    /**
     * Provides an instance of the requested filter.
     * @param id "name" of the filter to return.
     * @return Pointer to instance of the requested filter. The instance must be
     *         deleted when not needed anymore. Filter instances are never shared
     *         between filter chains.
     */
    FilterBase* instantiateFilter(const QString& id);

#ifdef SM_PRINT
    void print() const; // for debugging purposes only
#endif

    bool write(int id, const void* source, int size);

private Q_SLOTS:
    void lostClient(int sessionId);

    void displayStateChanged(const bool displayState);
    void devicePSMStateChanged(const bool deviceMode);
    void sensorDataHandler(int);

public:
    bool loadPlugin(const QString& name);

    int requestSensor(const QString& id);
    bool releaseSensor(const QString& id, int sessionId);
    const SensorInstanceEntry getSensorInstance(const QString& id) const { return sensorInstanceMap_.value(id); }
    SocketHandler& socketHandler() const { return *socketHandler_; };

Q_SIGNALS:
    void errorSignal(int error);
    void resumeCalibration();
    void stopCalibration();
    void displayOn();

protected:
    SensorManager();
    virtual ~SensorManager();

    void setError(SensorManagerError errorCode, const QString& errorString);
    void clearError() { errorCode_ = SmNoError; errorString_.clear(); }

    AbstractSensorChannel* addSensor(const QString& id);
    void removeSensor(const QString& id);

    QMap<QString, SensorChannelFactoryMethod>      sensorFactoryMap_;
    QMap<QString, SensorInstanceEntry>             sensorInstanceMap_;

    QMap<QString, DeviceAdaptorFactoryMethod>      deviceAdaptorFactoryMap_;
    QMap<QString, DeviceAdaptorInstanceEntry>      deviceAdaptorInstanceMap_;

    QMap<QString, ChainFactoryMethod>              chainFactoryMap_;
    QMap<QString, ChainInstanceEntry>              chainInstanceMap_;

    QMap<QString, FilterFactoryMethod>             filterFactoryMap_;

private:
    int createNewSessionId();
    QString socketToPid(int id) const;
    QString socketToPid(QList<int> ids) const;

    static SensorManager*                          instance_;

    SocketHandler*                                 socketHandler_;

    static int                                     sessionIdCount_;

    MceWatcher*                                    mceWatcher_;

    SensorManagerError                             errorCode_;
    QString                                        errorString_;
    bool                                           displayState_;
    bool                                           psmState_;
    int                                            pipefds_[2];
    QSocketNotifier*                               pipeNotifier_;
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
