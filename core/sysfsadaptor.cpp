/**
   @file sysfsadaptor.cpp
   @brief Base class for device adaptors reading data from files

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Shenghua <ext-shenghua.1.liu@nokia.com>

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
#include <unistd.h>
#include <QFile>
#include "logging.h"
#include "config.h"

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

bool SysfsAdaptor::isRunning() const
{
    return running_;
}

bool SysfsAdaptor::startAdaptor()
{
    sensordLogD() << "Starting adaptor: " << id();
    return true;
}

void SysfsAdaptor::stopAdaptor()
{
    sensordLogD() << "Stopping adaptor: " << id();
    if ( getAdaptedSensor()->isRunning() )
        stopSensor();
}

bool SysfsAdaptor::startSensor()
{
    AdaptedSensorEntry *entry = getAdaptedSensor();

    if (entry == NULL) {
        sensordLogW() << "Sensor not found: " << name();
        return false;
    }

    // Increase listener count
    entry->addReference();

    /// Check from entry
    if (isRunning()) {
        return false;
    }

    shouldBeRunning_ = true;

    // Do not open if in standby mode.
    if (inStandbyMode_ && !deviceStandbyOverride()) {
        return false;
    }

    /// We are waking up from standby or starting fresh, no matter
    inStandbyMode_ = false;

    if (!startReaderThread()) {
        sensordLogW() << "Failed to start adaptor " << name();
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

void SysfsAdaptor::stopSensor()
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
    sensordLogD() << "Adaptor '" << id() << "' requested to go to standby";
    if (inStandbyMode_) {
        sensordLogD() << "Adaptor '" << id() << "' not going to standby: already in standby";
        return false;
    }
    if (deviceStandbyOverride()) {
        sensordLogD() << "Adaptor '" << id() << "' not going to standby: overriden";
        return false;
    }
    inStandbyMode_ = true;

    if (!isRunning()) {
        sensordLogD() << "Adaptor '" << id() << "' not going to standby: not running";
        return false;
    }

    sensordLogD() << "Adaptor '" << id() << "' going to standby";
    stopReaderThread();
    closeAllFds();

    running_ = false;

    return true;
}

bool SysfsAdaptor::resume()
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

    if (!startReaderThread()) {
        sensordLogW() << "Adaptor '" << id() << "' failed to resume from standby!";
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
        if ((fd = open(paths_.at(i).toLatin1().constData(), O_RDONLY)) == -1) {
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
    }
    else
        reader_.stopReader();
    reader_.wait();
}

bool SysfsAdaptor::startReaderThread()
{
    if (!openFds()) {

        closeAllFds();
        return false;
    }

    reader_.startReader();

    return true;
}

bool SysfsAdaptor::writeToFile(const QByteArray& path, const QByteArray& content)
{
    sensordLogT() << "Writing to '" << path << ": " << content;
    if (!QFile::exists(path))
    {
        sensordLogW() << "Path does not exists: " << path;
        return false;
    }
    int fd = open(path.constData(), O_WRONLY);
    if (fd == -1)
    {
        sensordLogW() << "Failed to open '" << path << "': " << strerror(errno);
        return false;
    }

    if (write(fd, content.constData(), content.size() * sizeof(char)) == -1) {
        close(fd);
        return false;
    }

    close(fd);

    return true;
}

QByteArray SysfsAdaptor::readFromFile(const QByteArray& path)
{
    QFile file(path);
    if (!file.exists(path) || !(file.open(QIODevice::ReadOnly)))
    {
        sensordLogW() << "Path does not exists or open file failed: " << path;
        return QByteArray();
    }

    QByteArray data(file.readAll());
    sensordLogT() << "Read from '" << path << ": " << data;
    return data;
}

bool SysfsAdaptor::checkIntervalUsage() const
{
    if (mode_ == SysfsAdaptor::SelectMode)
    {
        const QList<DataRange>& list = getAvailableIntervals();
        if (list.size() > 1 || (list.size() == 1 && list.first().min != list.first().max))
        {
            sensordLogW() << "Attempting to use IntervalMode interval() function for adaptor in SelectMode. Must reimplement!";
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

SysfsAdaptor::PollMode SysfsAdaptor::mode() const
{
    return mode_;
}

SysfsAdaptorReader::SysfsAdaptorReader(SysfsAdaptor *parent) : running_(false), parent_(parent)
{
}

void SysfsAdaptorReader::stopReader()
{
    running_ = false;
}

void SysfsAdaptorReader::startReader()
{
    running_ = true;
    start();
}

void SysfsAdaptorReader::run()
{
    while (running_) {

        if (parent_->mode_ == SysfsAdaptor::SelectMode) {

            struct epoll_event events[parent_->sysfsDescriptors_.size() + 1];
            memset(events, 0x0, sizeof(events));

            int descriptors = epoll_wait(parent_->epollDescriptor_, events, parent_->sysfsDescriptors_.size() + 1, -1);

            if (descriptors == -1) {
                sensordLogD() << "epoll_wait(): " << strerror(errno);
                QThread::msleep(1000);
            } else {
                bool errorInInput = false;
                for (int i = 0; i < descriptors; ++i) {
                    if (events[i].events & (EPOLLHUP | EPOLLERR)) {
                        //Note: we ignore error so the sensordiverter.sh works. This should be handled better when testcases are improved.
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

void SysfsAdaptor::init()
{
    QString path = Config::configuration()->value(name() + "/path").toString();
    if(!path.isEmpty())
    {
        addPath(path);
    }
    else
    {
        sensordLogW() << "No sysfs path defined for: " << name();
    }
    mode_ = (PollMode)Config::configuration()->value<int>(name() + "/mode", mode_);
    doSeek_ = Config::configuration()->value<bool>(name() + "/seek", doSeek_);

    introduceAvailableDataRanges(name());
    introduceAvailableIntervals(name());
    setDefaultInterval(Config::configuration()->value<int>(name() + "/default_interval", 0));
}
