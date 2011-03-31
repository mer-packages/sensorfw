/**
   @file sensormanager.cpp
   @brief SensorManager

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>

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

#include "sensormanager_a.h"
#include "serviceinfo.h"
#include "sensormanager.h"
#include "loader.h"
#include "idutils.h"
#include "logging.h"
#include "mcewatcher.h"
#include "calibrationhandler.h"

#include <QSocketNotifier>
#include <errno.h>
#include "sockethandler.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct {
    int id;
    int size;
    void* buffer;
} PipeData;

SensorManager* SensorManager::instance_ = NULL;
int SensorManager::sessionIdCount_ = 0;

SensorInstanceEntry::SensorInstanceEntry(const QString& type) :
    sensor_(0),
    type_(type)
{
}

SensorInstanceEntry::~SensorInstanceEntry()
{
}

ChainInstanceEntry::ChainInstanceEntry(const QString& type) :
    cnt_(0),
    chain_(0),
    type_(type)
{
}

ChainInstanceEntry::~ChainInstanceEntry()
{
}

DeviceAdaptorInstanceEntry::DeviceAdaptorInstanceEntry(const QString& type, const QString& id) :
    adaptor_(0),
    cnt_(0),
    type_(type)
{
    propertyMap_ = ParameterParser::getPropertyMap(id);
}

DeviceAdaptorInstanceEntry::~DeviceAdaptorInstanceEntry()
{
}

inline QDBusConnection bus()
{
    return QDBusConnection::systemBus();
}

SensorManager& SensorManager::instance()
{
    if ( !instance_ )
    {
        instance_ = new SensorManager;
    }

    return *instance_;
}

SensorManager::SensorManager() :
    errorCode_(SmNoError),
    pipeNotifier_(0)
{
    const char* SOCKET_NAME = "/tmp/sensord.sock";

    new SensorManagerAdaptor(this);

    socketHandler_ = new SocketHandler(this);
    connect(socketHandler_, SIGNAL(lostSession(int)), this, SLOT(lostClient(int)));

    Q_ASSERT(socketHandler_->listen(SOCKET_NAME));

    if (pipe(pipefds_) == -1) {
        sensordLogC() << "Failed to create pipe: " << strerror(errno);
        pipefds_[0] = pipefds_[1] = 0;
    } else {
        pipeNotifier_ = new QSocketNotifier(pipefds_[0], QSocketNotifier::Read);
        connect(pipeNotifier_, SIGNAL(activated(int)), this, SLOT(sensorDataHandler(int)));
    }

    if (chmod(SOCKET_NAME, S_IRWXU|S_IRWXG|S_IRWXO) != 0) {
        sensordLogW() << "Error setting socket permissions! " << SOCKET_NAME;
    }

#ifdef SENSORFW_MCE_WATCHER

    mceWatcher_ = new MceWatcher(this);
    connect(mceWatcher_, SIGNAL(displayStateChanged(const bool)),
            this, SLOT(displayStateChanged(const bool)));

    connect(mceWatcher_, SIGNAL(devicePSMStateChanged(const bool)),
            this, SLOT(devicePSMStateChanged(const bool)));

#endif //SENSORFW_MCE_WATCHER
}

SensorManager::~SensorManager()
{
    foreach (const QString& key, sensorInstanceMap_.keys())
    {
         sensordLogW() << "ERROR: sensor " << key << " not released!";
    }

    foreach (const QString& key, deviceAdaptorInstanceMap_.keys())
    {
         sensordLogW() << "ERROR: device adaptor " << key << " not released!";
    }

    delete socketHandler_;
    delete pipeNotifier_;
    if (pipefds_[0]) close(pipefds_[0]);
    if (pipefds_[1]) close(pipefds_[1]);

#ifdef SENSORFW_MCE_WATCHER
    delete mceWatcher_;
#endif //SENSORFW_MCE_WATCHER
}

void SensorManager::setError(SensorManagerError errorCode, const QString& errorString)
{
    sensordLogW() << "SensorManagerError: " << errorString;

    errorCode_   = errorCode;
    errorString_ = errorString;

    emit errorSignal(errorCode);
}

bool SensorManager::registerService()
{
    clearError();

    bool ok = bus().isConnected();
    if ( !ok )
    {
        QDBusError error = bus().lastError();
        setError(SmNotConnected, error.message());
        return false;
    }

    ok = bus().registerObject( OBJECT_PATH, this );
    if ( !ok )
    {
        QDBusError error = bus().lastError();
        setError(SmCanNotRegisterObject, error.message());
        return false;
    }

    ok = bus().registerService ( SERVICE_NAME );
    if ( !ok )
    {
        QDBusError error = bus().lastError();
        setError(SmCanNotRegisterService, error.message());
        return false;
    }
    return true;
}

AbstractSensorChannel* SensorManager::addSensor(const QString& id)
{
    sensordLogD() << "Adding sensor: " << id;

    clearError();

    QString cleanId = getCleanId(id);
    QMap<QString, SensorInstanceEntry>::iterator entryIt = sensorInstanceMap_.find(cleanId);

    if (entryIt == sensorInstanceMap_.end()) {
        sensordLogC() << QString("%1 not present").arg(cleanId);
        setError( SmIdNotRegistered, QString(tr("instance for sensor type '%1' not registered").arg(cleanId)) );
        return NULL;
    }

    QString typeName = entryIt.value().type_;

    if ( !sensorFactoryMap_.contains(typeName) )
    {
        setError( SmFactoryNotRegistered, QString(tr("factory for sensor type '%1' not registered").arg(typeName)) );
        return NULL;
    }

    AbstractSensorChannel* sensorChannel = sensorFactoryMap_[typeName](id);
    if ( !sensorChannel->isValid() )
    {
        sensordLogC() << QString("%1 instantiation failed").arg(cleanId);
        delete sensorChannel;
        return NULL;
    }

    bool ok = bus().registerObject(OBJECT_PATH + "/" + sensorChannel->id(), sensorChannel);
    if ( !ok )
    {
        QDBusError error = bus().lastError();
        setError(SmCanNotRegisterObject, error.message());
        sensordLogC() << "Failed to register sensor '" << OBJECT_PATH + "/" + sensorChannel->id() << "'";
        delete sensorChannel;
        return NULL;
    }

    Q_ASSERT( entryIt.value().sensor_ == 0 );
    entryIt.value().sensor_ = sensorChannel;
    return sensorChannel;
}

void SensorManager::removeSensor(const QString& id)
{
    sensordLogD() << "Removing sensor: " << id;

    QMap<QString, SensorInstanceEntry>::iterator entryIt = sensorInstanceMap_.find(id);
    bus().unregisterObject(OBJECT_PATH + "/" + id);
    delete entryIt.value().sensor_;
    entryIt.value().sensor_ = 0;
}

bool SensorManager::loadPlugin(const QString& name)
{
    sensordLogD() << "Loading plugin: " << name;

    QString errorMessage;
    bool result;

    Loader& l = Loader::instance();
    if (! (result = l.loadPlugin(name, &errorMessage))) {
        setError (SmCanNotRegisterObject, errorMessage);
    }
    return result;
}

int SensorManager::requestSensor(const QString& id)
{
    sensordLogD() << "Requesting sensor: " << id;

    clearError();

    QString cleanId = getCleanId(id);
    QMap<QString, SensorInstanceEntry>::iterator entryIt = sensorInstanceMap_.find(cleanId);

    if ( entryIt == sensorInstanceMap_.end() )
    {
        setError(SmIdNotRegistered, QString(tr("requested sensor id '%1' not registered")).arg(cleanId));
        return INVALID_SESSION;
    }

    int sessionId = createNewSessionId();
    if(entryIt.value().sessions_.empty())
    {
        AbstractSensorChannel* sensor = addSensor(id);
        if ( sensor == NULL )
        {
            setError(SmNotInstantiated, tr("sensor has not been instantiated"));
            return INVALID_SESSION;
        }
    }
    entryIt.value().sessions_.insert(sessionId);

    return sessionId;
}

bool SensorManager::releaseSensor(const QString& id, int sessionId)
{
    sensordLogD() << "Releasing sensor '" << id << "' for session: " << sessionId;

    clearError();

    if( id.contains(';') ) // no parameter passing in release
    {
        sensordLogW() << "Invalid parameter passed to releaseSensor(): " << id;
        return false;
    }

    QMap<QString, SensorInstanceEntry>::iterator entryIt = sensorInstanceMap_.find(id);

    if ( entryIt == sensorInstanceMap_.end() )
    {
        setError( SmIdNotRegistered, QString(tr("requested sensor id '%1' not registered").arg(id)) );
        return false;
    }

    /// Remove any property requests by this session
    entryIt.value().sensor_->removeSession(sessionId);

    if (entryIt.value().sessions_.empty())
    {
        setError(SmNotInstantiated, tr("sensor has not been instantiated, no session to release"));
        return false;
    }

    bool returnValue = false;

    if(entryIt.value().sessions_.remove( sessionId ))
    {
        if ( entryIt.value().sessions_.empty() )
        {
            removeSensor(id);
        }
        returnValue = true;
    }
    else
    {
        // sessionId does not correspond to a request
        setError( SmNotInstantiated, tr("invalid sessionId, no session to release") );
    }

    socketHandler_->removeSession(sessionId);

    return returnValue;
}

AbstractChain* SensorManager::requestChain(const QString& id)
{
    sensordLogD() << "Requesting chain: " << id;
    clearError();

    AbstractChain* chain = NULL;
    QMap<QString, ChainInstanceEntry>::iterator entryIt = chainInstanceMap_.find(id);
    if (entryIt != chainInstanceMap_.end())
    {
        if (entryIt.value().chain_ )
        {
            chain = entryIt.value().chain_;
            entryIt.value().cnt_++;
            sensordLogD() << "Found chain '" << id << "'. Ref count: " << entryIt.value().cnt_;
        }
        else
        {
            QString type = entryIt.value().type_;
            if (chainFactoryMap_.contains(type))
            {
                chain = chainFactoryMap_[type](id);
                Q_ASSERT(chain);
                sensordLogD() << "Instantiated chain '" << id << "'. Valid = " << chain->isValid();

                entryIt.value().cnt_++;
                entryIt.value().chain_ = chain;
            }
            else
            {
                setError( SmFactoryNotRegistered, QString(tr("unknown chain type '%1'").arg(type)) );
            }
        }
    }
    else
    {
        setError( SmIdNotRegistered, QString(tr("unknown chain id '%1'").arg(id)) );
    }

    return chain;
}

void SensorManager::releaseChain(const QString& id)
{
    sensordLogD() << "Releasing chain: " << id;

    clearError();

    QMap<QString, ChainInstanceEntry>::iterator entryIt = chainInstanceMap_.find(id);
    if (entryIt != chainInstanceMap_.end())
    {
        if (entryIt.value().chain_)
        {
            entryIt.value().cnt_--;

            if (entryIt.value().cnt_ == 0) {
                sensordLogD() << "Chain '" << id << "' has no more references. Deleting it.";
                delete entryIt.value().chain_;
                entryIt.value().cnt_ = 0;
                entryIt.value().chain_ = 0;
            }
            else
            {
                sensordLogD() << "Chain '" << id << "' ref count: " << entryIt.value().cnt_;
            }
        }
        else
        {
            setError( SmNotInstantiated, QString(tr("chain '%1' not instantiated, cannot release").arg(id)) );
        }
    }
    else
    {
        setError( SmIdNotRegistered, QString(tr("unknown chain id '%1'").arg(id)) );
    }
}

DeviceAdaptor* SensorManager::requestDeviceAdaptor(const QString& id)
{
    sensordLogD() << "Requesting adaptor: " << id;

    clearError();
    if( id.contains(';') ) // no parameter passing in release
    {
        setError( SmIdNotRegistered, QString(tr("unknown adaptor id '%1'").arg(id)) );
        return false;
    }

    DeviceAdaptor* da = NULL;
    QMap<QString, DeviceAdaptorInstanceEntry>::iterator entryIt = deviceAdaptorInstanceMap_.find(id);
    if ( entryIt != deviceAdaptorInstanceMap_.end() )
    {
        if ( entryIt.value().adaptor_ )
        {
            Q_ASSERT( entryIt.value().adaptor_ );
            da = entryIt.value().adaptor_;
            entryIt.value().cnt_++;
            sensordLogD() << "Found adaptor '" << id << "'. Ref count: " << entryIt.value().cnt_;
        }
        else
        {
            QString type = entryIt.value().type_;
            if ( deviceAdaptorFactoryMap_.contains(type) )
            {
                da = deviceAdaptorFactoryMap_[type](id);
                Q_ASSERT( da );

                ParameterParser::applyPropertyMap(da, entryIt.value().propertyMap_);

                bool ok = da->startAdaptor();
                if (ok)
                {
                    entryIt.value().adaptor_ = da;
                    entryIt.value().cnt_++;
                    sensordLogD() << "Instantiated adaptor '" << id << "'. Valid = " << da->isValid();
                }
                else
                {
                    setError(SmAdaptorNotStarted, QString(tr("adaptor '%1' can not be started").arg(id)) );
                    delete da;
                    da = NULL;
                }
            }
            else
            {
                setError( SmFactoryNotRegistered, QString(tr("unknown adaptor type '%1'").arg(type)) );
            }
        }
    }
    else
    {
        setError( SmIdNotRegistered, QString(tr("unknown adaptor id '%1'").arg(id)) );
    }

    return da;
}

void SensorManager::releaseDeviceAdaptor(const QString& id)
{
    sensordLogD() << "Releasing adaptor: " << id;

    clearError();
    if( id.contains(';') ) // no parameter passing in release
    {
        setError( SmIdNotRegistered, QString(tr("unknown adaptor id '%1'").arg(id)) );
        return;
    }

    QMap<QString, DeviceAdaptorInstanceEntry>::iterator entryIt = deviceAdaptorInstanceMap_.find(id);
    if ( entryIt != deviceAdaptorInstanceMap_.end() )
    {
        if ( entryIt.value().adaptor_ )
        {
            Q_ASSERT( entryIt.value().adaptor_ );

            entryIt.value().cnt_--;
            if ( entryIt.value().cnt_ == 0 )
            {
                sensordLogD() << "Adaptor '" << id << "' has no more references. Deleting it.";

                Q_ASSERT( entryIt.value().adaptor_ );

                entryIt.value().adaptor_->stopAdaptor();

                delete entryIt.value().adaptor_;
                entryIt.value().adaptor_ = 0;
                entryIt.value().cnt_ = 0;
            }
            else
            {
                sensordLogD() << "Adaptor '" << id << "' has ref count: " << entryIt.value().cnt_;
            }
        }
        else
        {
            setError( SmNotInstantiated, QString(tr("adaptor '%1' not instantiated, cannot release").arg(id)) );
        }
    }
    else
    {
        setError( SmIdNotRegistered, QString(tr("unknown adaptor id '%1'").arg(id)) );
    }
}

FilterBase* SensorManager::instantiateFilter(const QString& id)
{
    sensordLogD() << "Instantiating filter: " << id;

    QMap<QString, FilterFactoryMethod>::iterator it = filterFactoryMap_.find(id);
    if(it == filterFactoryMap_.end())
    {
        sensordLogW() << "Filter " << id << " not found.";
        return NULL;
    }
    return it.value()();
}

bool SensorManager::write(int id, const void* source, int size)
{
    void* buffer = malloc(size);
    if(!buffer) {
        sensordLogC() << "Malloc failed!";
        return false;
    }
    PipeData pipeData;
    pipeData.id = id;
    pipeData.size = size;
    pipeData.buffer = buffer;

    memcpy(buffer, source, size);

    if (::write(pipefds_[1], &pipeData, sizeof(pipeData)) < (int)sizeof(pipeData)) {
        sensordLogW() << "Failed to write all data to pipe.";
        return false;
    }
    return true;
}

void SensorManager::sensorDataHandler(int)
{
    PipeData pipeData;
    read(pipefds_[0], &pipeData, sizeof(pipeData));

    if (!socketHandler_->write(pipeData.id, pipeData.buffer, pipeData.size)) {
        sensordLogW() << "Failed to write data to socket.";
    }

    free(pipeData.buffer);
}

void SensorManager::lostClient(int sessionId)
{
    for(QMap<QString, SensorInstanceEntry>::iterator it = sensorInstanceMap_.begin(); it != sensorInstanceMap_.end(); ++it) {
        if (it.value().sessions_.contains(sessionId)) {
            sensordLogD() << "[SensorManager]: Lost session " << sessionId << " detected as " << it.key();

            sensordLogD() << "[SensorManager]: Stopping sessionId " << sessionId;
            it.value().sensor_->stop(sessionId);

            sensordLogD() << "[SensorManager]: Releasing sessionId " << sessionId;
            releaseSensor(it.key(), sessionId);
            return;
        }
    }
    sensordLogW() << "[SensorManager]: Lost session " << sessionId << " detected, but not found from session list";
}

void SensorManager::displayStateChanged(bool displayState)
{
    sensordLogD() << "Signal detected, display state changed to: " << displayState;
    if (displayState) {
        /// Emit signal to make background calibration resume from sleep
        emit displayOn();
        if (!mceWatcher_->PSMEnabled())
        {
            emit resumeCalibration();
        }
    }

    foreach (const DeviceAdaptorInstanceEntry& adaptor, deviceAdaptorInstanceMap_) {
        if (adaptor.adaptor_) {
            if (displayState) {
                adaptor.adaptor_->setScreenBlanked(false);
                adaptor.adaptor_->resume();

            } else {
                adaptor.adaptor_->setScreenBlanked(true);
                adaptor.adaptor_->standby();
            }
        }
    }
}

void SensorManager::devicePSMStateChanged(bool psmState)
{
    if (psmState)
    {
        emit stopCalibration();
    }
}

void SensorManager::printStatus(QStringList& output) const
{
    output.append("  Adaptors:\n");
    for (QMap<QString, DeviceAdaptorInstanceEntry>::const_iterator it = deviceAdaptorInstanceMap_.constBegin(); it != deviceAdaptorInstanceMap_.constEnd(); ++it) {
        output.append(QString("    %1 [%2 listener(s)]\n").arg(it.value().type_).arg(it.value().cnt_));
    }

    output.append("  Chains:\n");
    for (QMap<QString, ChainInstanceEntry>::const_iterator it = chainInstanceMap_.constBegin(); it != chainInstanceMap_.constEnd(); ++it) {
        output.append(QString("    %1 [%2 listener(s)]. %3\n").arg(it.value().type_).arg(it.value().cnt_).arg((it.value().chain_ && it.value().chain_->running()) ? "Running" : "Stopped"));
    }

    output.append("  Logical sensors:\n");
    for (QMap<QString, SensorInstanceEntry>::const_iterator it = sensorInstanceMap_.constBegin(); it != sensorInstanceMap_.constEnd(); ++it) {

        QString str;
        str.append(QString("    %1 [").arg(it.value().type_));
        if(it.value().sessions_.size())
            str.append(QString("%1 session(s), PID(s): %2]").arg(it.value().sessions_.size()).arg(socketToPid(it.value().sessions_)));
        else
            str.append("No sessions]");
        str.append(QString(". %1\n").arg((it.value().sensor_ && it.value().sensor_->running()) ? "Running" : "Stopped"));
        output.append(str);
    }
}

QString SensorManager::socketToPid(int id) const
{
    struct ucred cr;
    socklen_t len = sizeof(cr);
    int fd = socketHandler_->getSocketFd(id);
    if (fd)
    {
        if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &cr, &len) == 0)
            return QString("%1").arg(cr.pid);
        else
            return strerror(errno);
    }
    return "n/a";
}

QString SensorManager::socketToPid(const QSet<int>& ids) const
{
    QString str;
    bool first = true;
    foreach (int id, ids)
    {
        if(!first)
            str.append(", ");
        first = false;
        str.append(socketToPid(id));
    }
    return str;
}

int SensorManager::createNewSessionId()
{
    return ++sessionIdCount_;
}

const SensorInstanceEntry* SensorManager::getSensorInstance(const QString& id) const
{
    QMap<QString, SensorInstanceEntry>::const_iterator it(sensorInstanceMap_.find(id));
    if(it == sensorInstanceMap_.end())
    {
        sensordLogW() << "Failed to locate sensor instance: " << id;
        return NULL;
    }
    return &it.value();
}

SensorManagerError SensorManager::errorCode() const
{
    return errorCode_;
}

int SensorManager::errorCodeInt() const
{
    return static_cast<int>(errorCode_);
}

const QString& SensorManager::errorString() const
{
    return errorString_;
}

void SensorManager::clearError()
{
    errorCode_ = SmNoError;
    errorString_.clear();
}

SocketHandler& SensorManager::socketHandler() const
{
    return *socketHandler_;
}

QList<QString> SensorManager::getAdaptorTypes() const
{
    return deviceAdaptorInstanceMap_.keys();
}

int SensorManager::getAdaptorCount(const QString& type) const
{
    QMap<QString, DeviceAdaptorInstanceEntry>::const_iterator it = deviceAdaptorInstanceMap_.find(type);
    if(it == deviceAdaptorInstanceMap_.end())
        return 0;
    return it.value().cnt_;
}

MceWatcher* SensorManager::MCEWatcher() const
{
    return mceWatcher_;
}

#ifdef SM_PRINT
void SensorManager::print() const
{
    sensordLogD() << "Registry Dump:";
    foreach(const QString& id, sensorInstanceMap_.keys())
    {
        sensordLogD() << "Registry entry id  =" << id;

        sensordLogD() << "sessions     =" << sensorInstanceMap_[id].sessions_;
        sensordLogD() << "sensor             =" << sensorInstanceMap_[id].sensor_;
        sensordLogD() << "type               =" << sensorInstanceMap_[id].type_ << endl;
    }

    sensordLogD() << "sensorInstanceMap(" << sensorInstanceMap_.size() << "):" << sensorInstanceMap_.keys();
    sensordLogD() << "sensorFactoryMap(" << sensorFactoryMap_.size() << "):" << sensorFactoryMap_.keys();
}
#endif
