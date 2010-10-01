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

#ifdef USE_SOCKET
#include <QSocketNotifier>
#include <errno.h>
#include "sockethandler.h"
#include <sys/stat.h>

#define SOCKET_NAME "/tmp/sensord.sock"

typedef struct {
    int id;
    int size;
    void* buffer;
} PipeData;
#endif

SensorManager* SensorManager::instance_ = NULL;
int SensorManager::sessionIdCount_ = 0;

inline
QDBusConnection bus()
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

SensorManager::SensorManager()
    : errorCode_(SmNoError)
{
    new SensorManagerAdaptor(this);

#ifdef USE_SOCKET
    socketHandler_ = new SocketHandler(this);
    connect(socketHandler_, SIGNAL(lostSession(int)), this, SLOT(lostClient(int)));

    Q_ASSERT(socketHandler_->listen(SOCKET_NAME));

    if (pipe(pipefds_) == -1) {
        sensordLogC() << strerror(errno);
        pipefds_[0] = pipefds_[1] = 0;
    } else {
        pipeNotifier_ = new QSocketNotifier(pipefds_[0], QSocketNotifier::Read);
        connect(pipeNotifier_, SIGNAL(activated(int)), this, SLOT(writeout(int)));
    }

    if (chmod(SOCKET_NAME, S_IRWXU|S_IRWXG|S_IRWXO) != 0) {
        sensordLogW() << "Error setting socket permissions! " << SOCKET_NAME;
    }
#endif

    connect(&propertyHandler_, SIGNAL(propertyRequestReceived(QString, QString)),
            this, SLOT(propertyRequest(QString, QString)));

#ifdef SENSORFW_MCE_WATCHER
    mceWatcher_ = new MceWatcher(this);
    connect(mceWatcher_, SIGNAL(displayStateChanged(const bool)),
            this, SLOT(displayStateChanged(const bool)));
#endif //SENSORFW_MCE_WATCHER
}

SensorManager::~SensorManager()
{
    foreach (QString key, sensorInstanceMap_.keys())
    {
         sensordLogW() << "ERROR: sensor" << key << "not released!";
         Q_ASSERT( sensorInstanceMap_[key].sensor_ == 0 );
    }

    foreach (QString key, deviceAdaptorInstanceMap_.keys())
    {
         sensordLogW() << "ERROR: device adaptor" << key << "not released!";
         Q_ASSERT( deviceAdaptorInstanceMap_[key].adaptor_ == 0 );
    }

#ifdef USE_SOCKET
    if (socketHandler_) {
        delete socketHandler_;
    }

    if (pipeNotifier_) delete pipeNotifier_;

    if (pipefds_[0]) close(pipefds_[0]);
    if (pipefds_[1]) close(pipefds_[1]);
#endif

#ifdef SENSORFW_MCE_WATCHER
    delete mceWatcher_;
#endif //SENSORFW_MCE_WATCHER
}

void SensorManager::setError(SensorManagerError errorCode, const QString& errorString)
{
    sensordLogW() << "SensorManagerError:" <<  errorString;

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

AbstractSensorChannel* SensorManager::addSensor(const QString& id, int sessionId, bool controllingSession)
{
    clearError();

    QString cleanId = getCleanId(id);

    if (!sensorInstanceMap_.contains(cleanId)) {
        sensordLogC() << QString("<%1> Sensor not present...").arg(cleanId);
        setError( SmIdNotRegistered, QString(tr("instance for sensor type '%1' not registered").arg(cleanId)) );
        return NULL;
    }

    QString typeName = sensorInstanceMap_[cleanId].type_;

    if ( !sensorFactoryMap_.contains(typeName) )
    {
        setError( SmFactoryNotRegistered, QString(tr("factory for sensor type '%1' not registered").arg(typeName)) );
        return NULL;
    }

    //sensordLogD() << "Creating sensor type:" << typeName << "id:" << id;
    AbstractSensorChannel* sensorChannel = sensorFactoryMap_[typeName](id);
    Q_ASSERT( sensorChannel );
    if ( sensorChannel->isValid() )
    {
        // TODO: why does this trap? does it?
        Q_ASSERT( sensorInstanceMap_[cleanId].sensor_ == 0 );
        sensorInstanceMap_[cleanId].sensor_ = sensorChannel;

        Q_ASSERT( sensorInstanceMap_[cleanId].listenSessions_.empty() );
        Q_ASSERT( sensorInstanceMap_[cleanId].controllingSession_ == INVALID_SESSION );

        if (controllingSession) {
            sensorInstanceMap_[cleanId].controllingSession_ = sessionId;
        } else {
            sensorInstanceMap_[cleanId].listenSessions_.append(sessionId);
        }

        // ToDo: decide whether SensorManager should really be D-BUS aware
        bool ok = bus().registerObject(OBJECT_PATH + "/" + sensorChannel->id(), sensorChannel);
        if ( !ok )
        {
            QDBusError error = bus().lastError();
            setError(SmCanNotRegisterObject, error.message());
            Q_ASSERT ( false ); // TODO: release the sensor and update administration accordingly...
            return NULL;
        }
    }
    else
    {
        delete sensorChannel;
        return NULL;
    }

    return sensorChannel;
}

void SensorManager::removeSensor(const QString& id)
{
    Q_ASSERT( sensorInstanceMap_[id].listenSessions_.empty() && sensorInstanceMap_[id].controllingSession_ == INVALID_SESSION);

    bus().unregisterObject(OBJECT_PATH + "/" + id);
    sensordLogD() << __PRETTY_FUNCTION__ << "object unregistered";
    sensordLogD() << __PRETTY_FUNCTION__ << "deleting" << sensorInstanceMap_[id].sensor_;
    delete sensorInstanceMap_[id].sensor_;
    sensordLogD() << __PRETTY_FUNCTION__ << "sensor instance deleted.";
    sensorInstanceMap_[id].sensor_ = 0;
}

bool SensorManager::loadPlugin(const QString& name)
{
    QString errorMessage;
    bool result;

    Loader& l = Loader::instance();
    if (! (result = l.loadPlugin(name, &errorMessage))) {
        setError (SmCanNotRegisterObject, errorMessage);
    }
    return result;
}

int SensorManager::requestControlSensor(const QString& id)
{
    clearError();

    QString cleanId = getCleanId(id);
    if ( !sensorInstanceMap_.contains( cleanId ) )
    {
        setError(SmIdNotRegistered, QString(tr("requested control sensor id '%1' not registered")).arg(cleanId));
        return INVALID_SESSION;
    }

    if ( sensorInstanceMap_[cleanId].controllingSession_ >= 0 )
    {
        setError(SmAlreadyUnderControl, tr("requested sensor already under control"));
        return INVALID_SESSION;
    }

    int sessionId = createNewSessionId();
    if ( sensorInstanceMap_[cleanId].listenSessions_.size() > 0 )
    {
        sensorInstanceMap_[cleanId].controllingSession_ = sessionId;
    }
    else
    {
        AbstractSensorChannel* sensor = addSensor(id, sessionId);
        if ( sensor == NULL )
        {
            return INVALID_SESSION;
        }
    }

    return sessionId;
}

int SensorManager::requestListenSensor(const QString& id)
{
    clearError();

    QString cleanId = getCleanId(id);
    if ( !sensorInstanceMap_.contains( cleanId ) )
    {
        setError( SmIdNotRegistered, QString(tr("requested listen sensor id '%1' not registered")).arg(cleanId) );
        return INVALID_SESSION;
    }

    int sessionId = createNewSessionId();
    if ( (sensorInstanceMap_[cleanId].listenSessions_.size() > 0) || (sensorInstanceMap_[cleanId].controllingSession_ >= 0) )
    {
        sensorInstanceMap_[cleanId].listenSessions_.append(sessionId);
    }
    else
    {
        AbstractSensorChannel* sensor = addSensor(id, sessionId, false);
        if ( sensor == NULL )
        {
            return INVALID_SESSION;
        }
    }

    return sessionId;
}

bool SensorManager::releaseSensor(const QString& id, int sessionId)
{
    Q_ASSERT( !id.contains(';') ); // no parameter passing in release

    clearError();

    if ( !sensorInstanceMap_.contains( id ) )
    {
        setError( SmIdNotRegistered, QString(tr("requested sensor id '%1' not registered").arg(id)) );
        return false;
    }

    /// Remove any property requests by this session
    propertyHandler_.clearRequests(sessionId);
    sensorInstanceMap_[id].sensor_->setStandbyOverrideRequest(sessionId, false);
    sensorInstanceMap_[id].sensor_->removeDataRangeRequest(sessionId);

    if ( (sensorInstanceMap_[id].controllingSession_ < 0) && (sensorInstanceMap_[id].listenSessions_.empty()))
    {
        setError(SmNotInstantiated, tr("sensor has not been instantiated, no session to release"));
        return false;
    }

    bool returnValue = false;
    // TODO: simplify this condition
    if ( sensorInstanceMap_[id].controllingSession_ >= 0 && sensorInstanceMap_[id].controllingSession_ == sessionId )
    {
        // sessionId corresponds to a control request
        if ( sensorInstanceMap_[id].listenSessions_.empty() )
        {
            // no listen sessions, sensor can be removed
            sensorInstanceMap_[id].controllingSession_ = INVALID_SESSION;
            removeSensor(id);
        }
        else
        {
            // listen sessions active, only remove control
            sensorInstanceMap_[id].controllingSession_ = INVALID_SESSION;
        }

        returnValue = true;
    }
    else
    {
        // sessionId does not correspond to a control request
        if ( sensorInstanceMap_[id].listenSessions_.contains( sessionId ) )
        {
            // sessionId does correspond to a listen request
            sensorInstanceMap_[id].listenSessions_.removeAll( sessionId );

            if ( sensorInstanceMap_[id].listenSessions_.empty() && sensorInstanceMap_[id].controllingSession_ == INVALID_SESSION )
            {
                removeSensor(id);
            }

            returnValue = true;
        }
        else
        {
            // sessionId does not correspond to a listen request
            setError( SmNotInstantiated, tr("invalid sessionId, no session to release") );
        }
    }

#ifdef USE_SOCKET
    // TODO: Release the socket, bind to single place
    socketHandler_->removeSession(sessionId);
#endif
    return returnValue;
}

AbstractChain* SensorManager::requestChain(const QString& id)
{
    clearError();

    AbstractChain* chain = NULL;
    if (chainInstanceMap_.contains(id)) {
        if (chainInstanceMap_[id].chain_ ) {
            Q_ASSERT (chainInstanceMap_[id].chain_);
            chain = chainInstanceMap_[id].chain_;
            chainInstanceMap_[id].cnt_++;
        } else {
            QString type = chainInstanceMap_[id].type_;
            if (chainFactoryMap_.contains(type)) {
                chain = chainFactoryMap_[type](id);
                Q_ASSERT(chain);

                chainInstanceMap_[id].cnt_++;
                chainInstanceMap_[id].chain_ = chain;
            } else {
                setError( SmFactoryNotRegistered, QString(tr("unknown chain type '%1'").arg(type)) );
            }
        }
    } else {
        setError( SmIdNotRegistered, QString(tr("unknown chain id '%1'").arg(id)) );
    }

    return chain;
}

void SensorManager::releaseChain(const QString& id)
{
    clearError();

    if (chainInstanceMap_.contains(id)) {
        if (chainInstanceMap_[id].chain_) {
            chainInstanceMap_[id].cnt_--;

            if (chainInstanceMap_[id].cnt_ == 0) {
                delete chainInstanceMap_[id].chain_;
                chainInstanceMap_[id].cnt_ = 0;
                chainInstanceMap_[id].chain_ = 0;
            }
        } else {
            setError( SmNotInstantiated, QString(tr("chain '%1' not instantiated, cannot release").arg(id)) );
        }
    } else {
        setError( SmIdNotRegistered, QString(tr("unknown chain id '%1'").arg(id)) );
    }
}

DeviceAdaptor* SensorManager::requestDeviceAdaptor(const QString& id)
{
    Q_ASSERT( !id.contains(';') ); // no parameter passing support for adaptors

    clearError();

    DeviceAdaptor* da = NULL;
    if ( deviceAdaptorInstanceMap_.contains(id) )
    {
        if ( deviceAdaptorInstanceMap_[id].adaptor_ )
        {
            Q_ASSERT( deviceAdaptorInstanceMap_[id].adaptor_ );
            //sensordLogD() << __PRETTY_FUNCTION__ << "instance exists already";
            da = deviceAdaptorInstanceMap_[id].adaptor_;
            deviceAdaptorInstanceMap_[id].cnt_++;
        }
        else
        {
            QString type = deviceAdaptorInstanceMap_[id].type_;
            if ( deviceAdaptorFactoryMap_.contains(type) )
            {
                sensordLogD() << __PRETTY_FUNCTION__ << "new instance created:" << id;
                da = deviceAdaptorFactoryMap_[type](id);
                Q_ASSERT( da );

                ParameterParser::applyPropertyMap(da, deviceAdaptorInstanceMap_[id].propertyMap_);

                bool ok = da->startAdaptor();
                if (ok)
                {
                    deviceAdaptorInstanceMap_[id].adaptor_ = da;
                    deviceAdaptorInstanceMap_[id].cnt_++;
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
    Q_ASSERT( !id.contains(';') ); // no parameter passing support for adaptors

    clearError();

    if ( deviceAdaptorInstanceMap_.contains(id) )
    {
        if ( deviceAdaptorInstanceMap_[id].adaptor_ )
        {
            Q_ASSERT( deviceAdaptorInstanceMap_[id].adaptor_ );

            deviceAdaptorInstanceMap_[id].cnt_--;
            //sensordLogD() << __PRETTY_FUNCTION__ << "new ref count" << deviceAdaptorInstanceMap_[id].cnt_;
            if ( deviceAdaptorInstanceMap_[id].cnt_ == 0 )
            {
                //sensordLogD() << __PRETTY_FUNCTION__ << "instance deleted";
                Q_ASSERT( deviceAdaptorInstanceMap_[id].adaptor_ );

                deviceAdaptorInstanceMap_[id].adaptor_->stopAdaptor();

                delete deviceAdaptorInstanceMap_[id].adaptor_;
                deviceAdaptorInstanceMap_[id].adaptor_ = 0;
                deviceAdaptorInstanceMap_[id].cnt_ = 0;
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
    FilterBase* filter = NULL;

    if (filterFactoryMap_.contains(id)) {
        filter = filterFactoryMap_[id]();
    } else {
        sensordLogW() << "Filter " << id << "not found.";
    }

    return filter;
}

#ifdef USE_SOCKET
bool SensorManager::write(int id, const void* source, int size)
{
    void* buffer = malloc(size);
    PipeData pipeData;

    if(!buffer) {
        sensordLogC() << "Malloc failed!";
        return false;
    }

    pipeData.id = id;
    pipeData.size = size;
    pipeData.buffer = buffer;

    memcpy(buffer, source, size);

    if (::write(pipefds_[1], &pipeData, sizeof(pipeData)) < (int)sizeof(pipeData)) {
        sensordLogW() << "Failed to write all data to pipe.";
        return false;
    }

    return true;
    // This used to be the old method, but must switch threads.
    // Thus using the writeout function through pipe.
    //return socketHandler_->write(id, source, size);
}

void SensorManager::writeout(int)
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
    foreach (QString id, sensorInstanceMap_.keys()) {
        if (sensorInstanceMap_[id].controllingSession_ == sessionId ||
            sensorInstanceMap_[id].listenSessions_.contains(sessionId)) {
            sensordLogD() << "[SensorManager]: Lost session detected as " << id << sessionId;

            sensordLogD() << "[SensorManager]: Stopping sessionId" << sessionId;
            sensorInstanceMap_[id].sensor_->stop(sessionId);

            sensordLogD() << "[SensorManager]: Releasing sessionId" << sessionId;
            releaseSensor(id, sessionId);
            return;
        }
    }
}
#endif

// TODO: Make the signal contain the new value (as long as int is always enough)
void SensorManager::propertyRequest(QString property, QString adaptor)
{
    int propertyValue = propertyHandler_.getHighestValue(property, adaptor);

    // Check that we have such an adaptor present:
    if (!deviceAdaptorInstanceMap_.contains(adaptor)) {
        sensordLogW() << "Setting property" << property << "for nonexisting adaptor" << adaptor;
    } else {
        deviceAdaptorInstanceMap_[adaptor].adaptor_->setProperty(property.toLatin1().constData(), (unsigned)propertyValue);
    }
}

void SensorManager::displayStateChanged(const bool displayState)
{
    sensordLogD() << "Signal detected, display state changed to:" << displayState;

    if (displayState) {
        /// Emit signal to make background calibration resume from sleep
        emit displayOn();
    }

    foreach (DeviceAdaptorInstanceEntry adaptor, deviceAdaptorInstanceMap_) {
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

void SensorManager::printStatus(QStringList& output) const
{
    output.append("  Adaptors:\n");
    foreach (QString key, deviceAdaptorInstanceMap_.keys()) {
        const DeviceAdaptorInstanceEntry& entry = deviceAdaptorInstanceMap_[key];
        output.append(QString("    %1 [%2 listener(s)]\n").arg(entry.type_).arg(entry.cnt_));
    }

    output.append("  Chains:\n");
    foreach (QString key, chainInstanceMap_.keys()) {
        const ChainInstanceEntry& entry = chainInstanceMap_[key];
        output.append(QString("    %1 [%2 listener(s)]. %3\n").arg(entry.type_).arg(entry.cnt_).arg(entry.chain_->running()?"Running":"Stopped"));
    }

    output.append("  Logical sensors:\n");
    foreach (QString key, sensorInstanceMap_.keys()) {
        const SensorInstanceEntry& entry = sensorInstanceMap_[key];
        bool control = true;
        if (entry.controllingSession_ <= 0) {
            control = false;
        }
        output.append(QString("    %1 [%2 %3 listen session(s)]. %4\n").arg(entry.type_).arg(control? "Control +":"No control,").arg(entry.listenSessions_.size()).arg(entry.sensor_->running()?"Running":"Stopped"));
    }
}

#ifdef SM_PRINT
void SensorManager::print() const
{
    sensordLogD() << "Registry Dump:";
    foreach(QString id, sensorInstanceMap_.keys())
    {
        sensordLogD() << "Registry entry id=" << id;

        sensordLogD() << "controllingSession_=" << sensorInstanceMap_[id].controllingSession_;
        sensordLogD() << "listenSessions_    =" << sensorInstanceMap_[id].listenSessions_;
        sensordLogD() << "sensor             =" << sensorInstanceMap_[id].sensor_;
        sensordLogD() << "type               =" << sensorInstanceMap_[id].type_ << endl;
    }

    sensordLogD() << "sensorInstanceMap_(" << sensorInstanceMap_.size() << "):" << sensorInstanceMap_.keys();
    sensordLogD() << "sensorFactoryMap(" << sensorFactoryMap_.size() << "):" << sensorFactoryMap_.keys();
}
#endif
