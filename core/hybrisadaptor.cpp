/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com
**
**
** $QT_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "hybrisadaptor.h"
#include "deviceadaptor.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTimer>

#include <android/hardware/hardware.h>
#include <android/hardware/sensors.h>

#ifndef SENSOR_TYPE_ACCELEROMETER
#define SENSOR_TYPE_ACCELEROMETER (1)
#endif
#ifndef SENSOR_TYPE_MAGNETIC_FIELD
#define SENSOR_TYPE_MAGNETIC_FIELD (2)
#endif
#ifndef SENSOR_TYPE_ORIENTATION
#define SENSOR_TYPE_ORIENTATION (3)
#endif
#ifndef SENSOR_TYPE_GYROSCOPE
#define SENSOR_TYPE_GYROSCOPE (4)
#endif
#ifndef SENSOR_TYPE_LIGHT
#define SENSOR_TYPE_LIGHT (5)
#endif
#ifndef SENSOR_TYPE_PRESSURE
#define SENSOR_TYPE_PRESSURE (6)
#endif
#ifndef SENSOR_TYPE_TEMPERATURE
#define SENSOR_TYPE_TEMPERATURE (7)
#endif
#ifndef SENSOR_TYPE_PROXIMITY
#define SENSOR_TYPE_PROXIMITY (8)
#endif
#ifndef SENSOR_TYPE_GRAVITY
#define SENSOR_TYPE_GRAVITY (9)
#endif
#ifndef SENSOR_TYPE_LINEAR_ACCELERATION
#define SENSOR_TYPE_LINEAR_ACCELERATION (10)
#endif
#ifndef SENSOR_TYPE_ROTATION_VECTOR
#define SENSOR_TYPE_ROTATION_VECTOR (11)
#endif
#ifndef SENSOR_TYPE_RELATIVE_HUMIDITY
#define SENSOR_TYPE_RELATIVE_HUMIDITY (12)
#endif
#ifndef SENSOR_TYPE_AMBIENT_TEMPERATURE
#define SENSOR_TYPE_AMBIENT_TEMPERATURE (13)
#endif
//#define SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED (14)
//#define SENSOR_TYPE_GAME_ROTATION_VECTOR (15)
//#define SENSOR_TYPE_GYROSCOPE_UNCALIBRATED (16)
//#define SENSOR_TYPE_SIGNIFICANT_MOTION (17)
//#define SENSOR_TYPE_STEP_DETECTOR (18)
//#define SENSOR_TYPE_STEP_COUNTER (19)
//#define SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR (20)

static QHash<QString,int> HybrisAdaptor_sensorTypes()
{
    QHash<QString,int> types;
    types["accelerometer"] = SENSOR_TYPE_ACCELEROMETER;
    types["magnetomoter"] = SENSOR_TYPE_MAGNETIC_FIELD;
    types["orientation"] = SENSOR_TYPE_ORIENTATION;
    types["gyroscope"] = SENSOR_TYPE_GYROSCOPE;
    types["light"] = SENSOR_TYPE_LIGHT;
    types["pressure"] = SENSOR_TYPE_PRESSURE;
    types["temperature"] = SENSOR_TYPE_TEMPERATURE;
    types["proximity"] = SENSOR_TYPE_PROXIMITY;
    types["gravity"] = SENSOR_TYPE_GRAVITY;
    types["lacceration"] = SENSOR_TYPE_LINEAR_ACCELERATION;
    return types;
}

Q_GLOBAL_STATIC(HybrisManager, hybrisManager)

HybrisManager::HybrisManager(QObject *parent) :
    QObject(parent),
    adaptorReader(parent),
    sensorsCount(0),
    sensorsOpened(0)
{
    init();
}

HybrisManager::~HybrisManager()
{
    closeAllSensors();
}

HybrisManager *HybrisManager::instance()
{
    HybrisManager *priv = hybrisManager();
    return priv;
}

void HybrisManager::init()
{
    int errorCode;

    errorCode = hw_get_module(SENSORS_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
    if (errorCode != 0) {
        qDebug() << "hw_get_module() failed" <<  strerror(-errorCode);
        return ;
    }

    errorCode = sensors_open(&module->common, &device);
    if (errorCode != 0) {
        qDebug() << "sensors_open() failed" <<  strerror(-errorCode);
        return ;
    }

    sensorsCount = module->get_sensors_list(module, &sensorList);

    for (int i = 0 ; i < sensorsCount ; i++) {
        sensorMap.insert(sensorList[i].type, i);
    }
}

int HybrisManager::handleForType(int sensorType)
{
    if (sensorMap.contains(sensorType))
        return sensorList[sensorMap[sensorType]].handle;
    return 0;
}

int HybrisManager::maxRange(int sensorType)
{
    if (sensorMap.contains(sensorType))
        return sensorList[sensorMap[sensorType]].maxRange;
    return 0;
}

int HybrisManager::minDelay(int sensorType)
{
    if (sensorMap.contains(sensorType))
        return sensorList[sensorMap[sensorType]].minDelay;
    return 0;
}

int HybrisManager::resolution(int sensorType)
{
    if (sensorMap.contains(sensorType))
        return sensorList[sensorMap[sensorType]].resolution;
    return 0;
}

bool HybrisManager::setDelay(int sensorHandle, int interval)
{
    int result = device->setDelay(device, sensorHandle, interval);
    if (result < 0) {
        sensordLogW() << "setDelay() failed" << strerror(-result);
        return false;
    }
    return true;
}

void HybrisManager::startReader(HybrisAdaptor *adaptor)
{
    qDebug() << Q_FUNC_INFO;
    if (registeredAdaptors.values().contains(adaptor)) {
        int error = device->activate(device, adaptor->sensorHandle, 1);
        if (error != 0) {
            sensordLogW() <<Q_FUNC_INFO<< "failed for"<< strerror(-error);
        }
        if (!adaptorReader.isRunning())
            adaptorReader.startReader();
    }
}

void HybrisManager::stopReader(HybrisAdaptor *adaptor)
{
    QList <HybrisAdaptor *> list;
    list = registeredAdaptors.values();
    bool okToStop = true;

    for (int i = 0; i < list.count(); i++) {
        if (list.at(i) != adaptor && list.at(i)->isRunning()) {
            okToStop = false;
        }
    }
    if (okToStop) {
        adaptorReader.stopReader();
        adaptorReader.wait();
        int error = device->activate(device, adaptor->sensorHandle, 0);
        if (error != 0) {
            sensordLogW() <<Q_FUNC_INFO<< "failed for"<< strerror(-error);
        }
    }
}

bool HybrisManager::resumeReader(HybrisAdaptor *adaptor)
{
    sensordLogD() << Q_FUNC_INFO << adaptor->id() << adaptor->deviceStandbyOverride();
    if (!adaptor->deviceStandbyOverride()) {
        setDelay(adaptor->sensorHandle, adaptor->interval() * 1000000);
    } else {
        int error = device->activate(device, adaptor->sensorHandle, 1);
        if (error != 0) {
            sensordLogW() <<Q_FUNC_INFO<< "failed for"<< strerror(-error);
        }
    }
    return true;
}

void HybrisManager::standbyReader(HybrisAdaptor *adaptor)
{
    sensordLogD() << Q_FUNC_INFO << adaptor->id() << adaptor->deviceStandbyOverride();
    if (!adaptor->deviceStandbyOverride()) {
        setDelay(adaptor->sensorHandle, 1 * 1000000);
    } else {
        int error = device->activate(device, adaptor->sensorHandle, 0);
        if (error != 0) {
            sensordLogW() <<Q_FUNC_INFO<< "failed for"<< strerror(-error);
        }

    }
}

bool HybrisManager::openSensors()
{
    if (!sensorsOpened) {
        int errorCode = sensors_open(&module->common, &device);
        if (errorCode != 0) {
            qDebug() << "sensors_open() failed" <<  strerror(-errorCode);
            return false;
        }
    }
    sensorsOpened = true;
    return true;
}

bool HybrisManager::closeSensors()
{
    QList <HybrisAdaptor *> list;
    list = registeredAdaptors.values();
    bool okToStop = true;

    for (int i = 0; i < list.count(); i++) {
        if (list.at(i)->isRunning())
            okToStop = false;
        qDebug() << Q_FUNC_INFO << "still running"<< list.at(i);
    }
    if (okToStop) {

        if (sensorsOpened) { //TODO
            int errorCode = sensors_close(device);
            if (errorCode != 0) {
                qDebug() << "sensors_close() failed" << strerror(-errorCode);
                return false;
            }
        }
    }
    return true;
}

void HybrisManager::closeAllSensors()
{
    QList <HybrisAdaptor *> list;
    list = registeredAdaptors.values();

    for (int i = 0; i < list.count(); i++) {
        if (list.at(i)->isRunning())
            list.at(i)->stopSensor();
    }
    int errorCode = sensors_close(device);
    if (errorCode != 0) {
        qDebug() << "sensors_close() failed" << strerror(-errorCode);
    }
}

void HybrisManager::processSample(const sensors_event_t& data)
{

    if (registeredAdaptors.contains(data.type)) {
        QList <HybrisAdaptor *> list;
        list = registeredAdaptors.values(data.type);
        for (int i = 0; i < list.count(); i++){
            if (list.at(i)->isRunning())
                list.at(i)->processSample(data);
        }
    }
}

void HybrisManager::registerAdaptor(HybrisAdaptor *adaptor)
{
    if (!registeredAdaptors.values().contains(adaptor)) {
        registeredAdaptors.insertMulti(adaptor->sensorType, adaptor);
    }
}

//////////////////////////////////
HybrisAdaptor::HybrisAdaptor(const QString& id, int type)
    : DeviceAdaptor(id),
      sensorType(type),
      cachedInterval(50),
      inStandbyMode_(false),
      running_(false)
{
    if (!HybrisAdaptor_sensorTypes().values().contains(sensorType)) {
        qDebug() << Q_FUNC_INFO <<"no such sensor" << id;
        return;
    }

    hybrisManager()->registerAdaptor(this);
    init();
    introduceAvailableInterval(DataRange(minDelay, 1000, 0));
    introduceAvailableDataRange(DataRange(-(maxRange*.5), (maxRange*.5), 1));
}

HybrisAdaptor::~HybrisAdaptor()
{
}

void HybrisAdaptor::init()
{
    sensorHandle = hybrisManager()->handleForType(sensorType);
    maxRange = hybrisManager()->maxRange(sensorType);
    minDelay = hybrisManager()->minDelay(sensorType);
    resolution = hybrisManager()->resolution(sensorType);
}

bool HybrisAdaptor::addSensorType(int type)
{
    sensorIds.append(type);
    return true;
}

bool HybrisAdaptor::startAdaptor()
{
    return hybrisManager()->openSensors();
}

bool HybrisAdaptor::isRunning() const
{
    return running_;
}

void HybrisAdaptor::stopAdaptor()
{
    if (getAdaptedSensor()->isRunning())
        stopSensor();
    hybrisManager()->closeSensors();
}

bool HybrisAdaptor::startSensor()
{
    AdaptedSensorEntry *entry = getAdaptedSensor();
    if (entry == NULL) {
        qDebug() << "Sensor not found: " << name();
        return false;
    }

    // Increase listener count
    entry->addReference();

    /// Check from entry
    if (isRunning()) {
        qDebug()  << Q_FUNC_INFO << "already running";
        return false;
    }

    // Do not open if in standby mode.
    if (inStandbyMode_ && !deviceStandbyOverride()) {
        qDebug()  << Q_FUNC_INFO << "inStandbyMode_ true";
        return false;
    }

    shouldBeRunning_ = true;
    /// We are waking up from standby or starting fresh, no matter
    inStandbyMode_ = false;

    if (!startReaderThread()) {
        entry->removeReference();
        entry->setIsRunning(false);
        running_ = false;
        shouldBeRunning_ = false;
        return false;
    }

    entry->setIsRunning(true);
    running_ = true;

    return true;
}

void HybrisAdaptor::stopSensor()
{
    AdaptedSensorEntry *entry = getAdaptedSensor();

    if (entry == NULL) {
        sensordLogW() << "Sensor not found " << name();
        return;
    }

    if (!shouldBeRunning_) {
        return;
    }

    entry->removeReference();
    if (entry->referenceCount() <= 0) {
        entry->setIsRunning(false);
        running_ = false;
        shouldBeRunning_ = false;
        if (!inStandbyMode_) {
            stopReaderThread();
        }
    }
}


bool HybrisAdaptor::standby()
{
    sensordLogD() << "Adaptor '" << id() << "' requested to go to standby";
    if (inStandbyMode_) {
        sensordLogD() << "Adaptor '" << id() << "' not going to standby: already in standby";
        return false;
    }

    if (!isRunning()) {
        sensordLogD() << "Adaptor '" << id() << "' not going to standby: not running";
        return false;
    }

    inStandbyMode_ = true;
    sensordLogD() << "Adaptor '" << id() << "' going to standby";
    hybrisManager()->standbyReader(this);

    running_ = !deviceStandbyOverride();
    return true;
}


bool HybrisAdaptor::resume()
{
    sensordLogD() << "Adaptor '" << id() << "' requested to resume from standby";

    // Don't resume if not in standby
    if (!inStandbyMode_) {
        sensordLogD() << "Adaptor '" << id() << "' not resuming: not in standby";
        return false;
    }

    inStandbyMode_ = false;

    if (!shouldBeRunning_) {
        sensordLogD() << "Adaptor '" << id() << "' not resuming from standby: not running";
        return false;
    }

    sensordLogD() << "Adaptor '" << id() << "' resuming from standby";

    if (!hybrisManager()->resumeReader(this)) {
        sensordLogW() << "Adaptor '" << id() << "' failed to resume from standby!";
        return false;
    }

    running_ = true;
    return true;
}

unsigned int HybrisAdaptor::interval() const
{
    return cachedInterval;
}

bool HybrisAdaptor::setInterval(const unsigned int value, const int /*sessionId*/)
{                     // 1000000
    cachedInterval = value;
    bool ok;
    qreal ns = value * 1000000; // ms to ns
    ok = hybrisManager()->setDelay(sensorHandle, ns);
    if (!ok) {
        qDebug() << Q_FUNC_INFO << "setInterval not ok";
    }
    return ok;
}

void HybrisAdaptor::stopReaderThread()
{
    hybrisManager()->stopReader(this);
    running_ = false;
}

bool HybrisAdaptor::startReaderThread()
{
    running_ = true;
    hybrisManager()->startReader(this);
    return true;
}

unsigned int HybrisAdaptor::evaluateIntervalRequests(int& sessionId) const
{
    if (m_intervalMap.size() == 0)
    {
        sessionId = -1;
        return defaultInterval();
    }

    // Get the smallest positive request, 0 is reserved for HW wakeup
    QMap<int, unsigned int>::const_iterator it = m_intervalMap.constBegin();
    unsigned int highestValue = it.value();
    int winningSessionId = it.key();

    for (++it; it != m_intervalMap.constEnd(); ++it)
    {
        if (((it.value() < highestValue) && (it.value() > 0)) || highestValue == 0) {
            highestValue = it.value();
            winningSessionId = it.key();
        }
    }

    sessionId = winningSessionId;
    return highestValue > 0 ? highestValue : defaultInterval();
}

/*/////////////////////////////////////////////////////////////////////
/// \brief HybrisAdaptorReader::HybrisAdaptorReader
/// \param parent
///
*/

HybrisAdaptorReader::HybrisAdaptorReader(QObject *parent)
    : QThread(parent),
    running_(false)
{
}

HybrisAdaptorReader::~HybrisAdaptorReader()
{
}

////
/// \brief HybrisAdaptorReader::stopReader
///
void HybrisAdaptorReader::stopReader()
{
    running_ = false;
}

void HybrisAdaptorReader::startReader()
{
    running_ = true;
    start();
}

void HybrisAdaptorReader::run()
{
    int err = 0;
    static const size_t numEvents = 16;
    sensors_event_t buffer[numEvents];

    while (running_) {
        int numberOfEvents = hybrisManager()->device->poll(hybrisManager()->device, buffer, numEvents);
        if (numberOfEvents < 0) {
            sensordLogW() << "poll() failed" << strerror(-err);
            QThread::msleep(1000);
        } else {
            bool errorInInput = false;

            for (int i = 0; i < numberOfEvents; i++) {
                const sensors_event_t& data = buffer[i];

                if (data.version != sizeof(sensors_event_t)) {
                    sensordLogW()<< QString("incorrect event version (version=%1, expected=%2").arg(data.version).arg(sizeof(sensors_event_t));
                    errorInInput = true;
                }
                hybrisManager()->processSample(data);

            }
            if (errorInInput)
                QThread::msleep(50);
        }
    }
    sensordLogT() << Q_FUNC_INFO << "runner thread end";

}

////////////////////////////////////////////////////////
