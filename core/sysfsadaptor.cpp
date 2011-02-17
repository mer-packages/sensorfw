/**
   @file sysfsadaptor.cpp
   @brief Base class for device adaptors reading data from files

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#include "sysfsadaptor.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <QFile>
#include "logging.h"

SysfsAdaptor::SysfsAdaptor(const QString& id,
                           PollMode mode,
                           bool seek,
                           const QString& path,
                           const int pathId) :
    DeviceAdaptor(id),
    reader_(this),
    mode_(mode),
    epollDescriptor_(-1),
    interval_(0),
    initNotDone(true),
    inStandbyMode_(false),
    running_(false),
    shouldBeRunning_(false),
    doSeek_(seek)
{
    if (!path.isEmpty()) {
        addPath(path, pathId);
    }

    pipeDescriptors_[0] = -1;
    pipeDescriptors_[1] = -1;
}

SysfsAdaptor::~SysfsAdaptor()
{
    stopAdaptor();
}

bool SysfsAdaptor::addPath(const QString& path, const int id)
{
    if (!QFile::exists(path)) {
        return false;
    }

    paths_.append(path);
    pathIds_.append(id);

    return true;
}

bool SysfsAdaptor::isRunning()
{
    return running_;
}

bool SysfsAdaptor::startAdaptor()
{
    if (initNotDone) {
        connect(&reader_, SIGNAL(readyRead(int, int)),
                this, SLOT(dataAvailable(int, int)));
        initNotDone = false;
    }
    // Nothing to initialize. File handles are opened on HW sensor start,
    // otherwise we might not get data on restart.
    return true;
}

void SysfsAdaptor::stopAdaptor()
{
    if(initNotDone)
        return;
    initNotDone = true;
    disconnect(&reader_, SIGNAL(readyRead(int,int)), this, SLOT(dataAvailable(int,int)));

    foreach (AdaptedSensorEntry* entry, sensors()) {
        if ( entry->isRunning() ) {
            //TODO: Drop refcount, so that we will definitely stop.
            stopSensor(entry->name());
        }
    }
}

bool SysfsAdaptor::startSensor(const QString& sensorId)
{
    AdaptedSensorEntry *entry = findAdaptedSensor(sensorId);

    if (entry == NULL) {
        sensordLogW() << "Sensor not found " << sensorId;
        return false;
    }

    // Increase listener count
    entry->addReference();

    /// Check from entry
    if (isRunning()) {
        return true;
    }

    shouldBeRunning_ = true;

    // Do not open if in standby mode.
    if (inStandbyMode_ && !deviceStandbyOverride()) {
        return true;
    }

    /// We are waking up from standby or starting fresh, no matter
    inStandbyMode_ = false;

    if (!startReaderThread()) {
        sensordLogW() << "Failed to start adaptor " << sensorId;
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

void SysfsAdaptor::stopSensor(const QString& sensorId)
{
    AdaptedSensorEntry *entry = findAdaptedSensor(sensorId);

    if (entry == NULL) {
        sensordLogW() << "Sensor not found" << sensorId;
        return;
    }

    if (!shouldBeRunning_) {
        return;
    }

    entry->removeReference();
    if (entry->referenceCount() <= 0) {
        if (!inStandbyMode_) {
            stopReaderThread();
            closeAllFds();
        }
        entry->setIsRunning(false);
        running_ = false;
        shouldBeRunning_ = false;
    }
}

bool SysfsAdaptor::standby()
{
    if (deviceStandbyOverride() || inStandbyMode_) {
        return true;
    }
    inStandbyMode_ = true;

    if (!isRunning()) {
        sensordLogD() << id() << " not going to standby: not running";
        return true;
    }

    sensordLogD() << id() << " going to standby";
    stopReaderThread();
    closeAllFds();

    running_ = false;

    return true;
}

bool SysfsAdaptor::resume()
{
    // Don't resume if not in standby
    if (!inStandbyMode_) {
        return true;
    }

    inStandbyMode_ = false;

    if (!shouldBeRunning_) {
        sensordLogD() << id() << " not resuming from standby: not running";
        return true;
    }

    sensordLogD() << id() << " resuming from standby";

    if (!startReaderThread()) {
        sensordLogW() << "Failed to resume " << id() << " from standby!";
        return false;
    }

    running_ = true;
    return true;
}

bool SysfsAdaptor::openFds()
{
    QMutexLocker locker(&mutex_);

    int fd;
    for (int i = 0; i < paths_.size(); i++) {
        if ((fd = open(paths_.at(i).toAscii().constData(), O_RDONLY)) == -1) {
            sensordLogW() << "open(): " << strerror(errno);
            return false;
        }
        sysfsDescriptors_.append(fd);
    }

    // Set up epoll for select mode
    if (mode_ == SelectMode) {

        if (pipe(pipeDescriptors_) == -1 ) {
            sensordLogW() << "pipe(): " << strerror(errno);
            return false;
        }

        if (fcntl(pipeDescriptors_[0], F_SETFD, FD_CLOEXEC) == -1) {
            sensordLogW() << "fcntl(): " << strerror(errno);
            return false;
        }

        // Set up epoll fd
        if ((epollDescriptor_ = epoll_create(sysfsDescriptors_.size() + 1)) == -1) {
            sensordLogW() << "epoll_create(): " << strerror(errno);
            return false;
        }

        struct epoll_event ev;
        memset(&ev, 0, sizeof(epoll_event));
        ev.events  = EPOLLIN;

        // Set up epolling for the list
        for (int i = 0; i < sysfsDescriptors_.size(); ++i) {
            ev.data.fd = sysfsDescriptors_.at(i);
            if (epoll_ctl(epollDescriptor_, EPOLL_CTL_ADD, sysfsDescriptors_.at(i), &ev) == -1) {
                sensordLogW() << "epoll_ctl(): " << strerror(errno);
                return false;
            }
        }

        // Add control pipe to poll list
        ev.data.fd = pipeDescriptors_[0];
        if (epoll_ctl(epollDescriptor_, EPOLL_CTL_ADD, pipeDescriptors_[0], &ev) == -1) {
            sensordLogW() << "epoll_ctl(): " << strerror(errno);
            return false;
        }
    }

    return true;
}

void SysfsAdaptor::closeAllFds()
{
    QMutexLocker locker(&mutex_);

    /* Epoll */
    if (epollDescriptor_ != -1) {
        close(epollDescriptor_);
        epollDescriptor_ = -1;
    }

    /* Pipe */
    for (int i = 0; i < 2; ++i) {
        if (pipeDescriptors_[i] != -1) {
            close(pipeDescriptors_[i]);
            pipeDescriptors_[i] = -1;
        }
    }

    /* SysFS */
    while (!sysfsDescriptors_.empty()) {
        if (sysfsDescriptors_.last() != -1) {
            close(sysfsDescriptors_.last());
        }
        sysfsDescriptors_.removeLast();
    }
}

void SysfsAdaptor::stopReaderThread()
{
    if (mode_ == SelectMode) {
        quint64 dummy = 1;
        write(pipeDescriptors_[1], &dummy, 8);
    } else {
        reader_.running_= false;
    }

    reader_.wait();
}

bool SysfsAdaptor::startReaderThread()
{
    if (!openFds()) {

        closeAllFds();
        return false;
    }

    reader_.running_ = true;
    reader_.start();

    return true;
}

bool SysfsAdaptor::writeToFile(QString path, QString content)
{
    if (!QFile::exists(path)) {
        return false;
    }

    int fd = open(path.toLocal8Bit().constData(), O_WRONLY);
    if (fd == -1) {
        return false;
    }

    if (write(fd, content.toLocal8Bit().constData(), content.toLocal8Bit().size() * sizeof(char)) == -1) {
        close(fd);
        return false;
    }

    close(fd);

    return true;
}

void SysfsAdaptor::dataAvailable(int pathId, int fd)
{
    processSample(pathId, fd);
}

bool SysfsAdaptor::checkIntervalUsage() const
{
    if (mode_ == SysfsAdaptor::SelectMode)
    {
        const QList<DataRange>& list = getAvailableIntervals();
        if (list.size() > 1 || (list.size() == 1 && list.first().min != list.first().max))
        {
            sensordLogW() << "Attempting to use PollMode interval() function for adaptor in SelectMode. Must reimplement!";
            return false;
        }
    }
    return true;
}

unsigned int SysfsAdaptor::interval() const
{
    if(!checkIntervalUsage())
        return 0;
    return interval_;
}

bool SysfsAdaptor::setInterval(const unsigned int value, const int sessionId)
{
    Q_UNUSED(sessionId);
    if(!checkIntervalUsage())
        return false;
    interval_ = value;
    return true;
}

SysfsAdaptorReader::SysfsAdaptorReader(SysfsAdaptor *parent) : running_(false), parent_(parent)
{
}

void SysfsAdaptorReader::run()
{
    while (running_) {

        if (parent_->mode_ == SysfsAdaptor::SelectMode) {

            struct epoll_event events[parent_->sysfsDescriptors_.size() + 1];
            memset(events, 0x0, sizeof(events));

            int descriptors = epoll_wait(parent_->epollDescriptor_, events, parent_->sysfsDescriptors_.size() + 1, -1);

            if (descriptors == -1) {
                // TODO: deal with errors
                sensordLogD() << "epoll_wait(): " << strerror(errno);
                QThread::msleep(1000);
            } else {
                bool errorInInput = false;
                for (int i = 0; i < descriptors; ++i) {
                    if (events[i].events & (EPOLLHUP | EPOLLERR)) { //TODO: we sort of ignore these type of errors to get sensordiverter.sh working
                        sensordLogD() << "epoll_wait(): error in input fd";
                        errorInInput = true;
                    }
                    int index = parent_->sysfsDescriptors_.lastIndexOf(events[i].data.fd);
                    if (index != -1) {
                        parent_->processSample(parent_->pathIds_.at(index), events[i].data.fd);

                        if (parent_->doSeek_)
                        {
                            if (lseek(events[i].data.fd, 0, SEEK_SET) == -1)
                            {
                                sensordLogW() << "Failed to lseek fd: " << strerror(errno);
                                QThread::msleep(1000);
                            }
                        }
                    } else if (events[i].data.fd == parent_->pipeDescriptors_[0]) {
                        running_ = false;
                    }
                }
                if (errorInInput)
                    QThread::msleep(50);
            }
        } else { //IntervalMode

            // Read through all fds.
            for (int i = 0; i < parent_->sysfsDescriptors_.size(); ++i) {

                parent_->processSample(parent_->pathIds_.at(i), parent_->sysfsDescriptors_.at(i));

                if (parent_->doSeek_)
                {
                    if (lseek(parent_->sysfsDescriptors_.at(i), 0, SEEK_SET) == -1)
                    {
                        sensordLogW() << "Failed to lseek fd: " << strerror(errno);
                        QThread::msleep(1000);
                    }
                }
            }

            // Sleep for interval
            QThread::msleep(parent_->interval());
        }
    }
}
