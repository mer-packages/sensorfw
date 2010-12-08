/**
   @file sysfsadaptor.h
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

#ifndef SYSFSADAPTOR_H
#define SYSFSADAPTOR_H

#include "sensord/deviceadaptor.h"
#include "sensord/deviceadaptorringbuffer.h"
#include <QString>
#include <QStringList>
#include <QThread>
#include <sensord/filterproperty.h>
#include <QMutex>

class SysfsAdaptor;

/**
 * Threading functionality for SysfsAdaptor. Helper class that removes ambiguous
 * inheritance from QObject. Should not be invoked directly by anything except
 * #SysfsAdaptor.
 */
class SysfsAdaptorReader : public QThread
{
    Q_OBJECT;
public:
    SysfsAdaptorReader(SysfsAdaptor *parent);
    void run();
    bool running_;

Q_SIGNALS:
    void readyRead(const int pathId, const int fd);

private:
    SysfsAdaptor *parent_;
};

/**
 * @brief Base class for adaptors accessing device drivers through sysfs.
 *
 * Two different polling modes are supported:
 * <ul>
 *   <li><tt>SysfsAdaptor::IntervalMode</tt> - Read constantly by given frequency (ms delay between reads).</li>
 *   <li><tt>SysfsAdaptor::SelectMode</tt>   - Wait for interrupt from driver before reading.</li>
 * </ul>
 *
 * Simultaneous monitoring of several files is supported. Currently files are indexed
 * only by their adding order. First added path is index 0, second index 1, etc.
 *
 * @todo Add resonable error handling
 * @todo Convert activity handling to use function pointers instead in indices.
 */
class SysfsAdaptor : public DeviceAdaptor, public PropertyTracker
{
    Q_OBJECT;

public:
    enum PollMode {
        SelectMode = 0, /**< Wait for interrupt from driver before reading. */
        IntervalMode    /**< Read constantly with given frequency. */
    };

    /**
     * Constructor.
     * @param id   Identifier string for the adaptor.
     * @param mode Mode to use for monitoring.
     * @param seek Whether lseek() should be called to rewind the monitored fds.
     * @param path Path to the sysfs file device to monitor.
     * @param id   Identifier for the path (used as parameter to processSample).
     */
    SysfsAdaptor(const QString& id, PollMode mode = SelectMode, bool seek = true, const QString& path = "", const int pathId = 0);

    /**
     * Add a new file device for monitoring. Adaptor must be restarted to
     * get the newly added path into monitoring list.
     * @param path Path to the sysfs file device to monitor.
     * @param id   Identified for the path (used as parameter to processSample).
     * @return     True on success, false otherwise.
     */
    bool addPath(const QString& path, const int id = 0);

    /**
     * Start measuring loop. Opens file descriptors.
     */
    bool startAdaptor();

    /**
     * Returns whether the adaptor is running or not.
     * isRunning() returns false if all the started adaptor references
     * have been stopped.
     */
    bool isRunning();

    /**
     * Stop measuring loop. Closes file descriptors.
     */
    void stopAdaptor();

    virtual bool startSensor(const QString& sensorId);
    virtual void stopSensor(const QString& sensorId);

    /**
     * See #DeviceAdaptor::standby()
     */
    virtual bool standby();

    /**
     * See #DeviceAdaptor::resume()
     */
    virtual bool resume();

    /**
     * Called when new data is available on some file descriptor. Must be implemented
     * by the child class.
     * @param pathId Path ID for the file that has received new data. If path ID
     *               was not set when file path was added, 0 will be used.
     * @param fd     Open file descriptor where the new data can be read. This
     *               file descriptor must not be closed!
     */
    virtual void processSample(int pathId, int fd) = 0;

protected slots:
    void dataAvailable(int pathId, int fd);

protected:
    /**
     * Utility function for writing to files. Can be used to control sensor driver
     * parameters (setting to powersave mode etc.)
     *
     * @param path    Path of the file to write to
     * @param content What to write
     * @return True on success, false on failure.
     */
    bool writeToFile(QString path, QString content);

    /**
     * Returns the current interval (see setInterval()). Valid for PollMode.
     * Reimplementation for adaptors using SelectMode is a must.
     *
     * @return Currently used interval for adaptor.
     */
    virtual unsigned int interval() const;

    /**
     * Sets the interval for the adaptor. This function is valid for
     * adaptors using PollMode. It just sets the number of milliseconds
     * that the adaptor will sleep between reads.
     *
     * For adaptors using SelectMode, reimplementation is a must as this
     * implementatino will have no effect on the behavior.
     *
     * @param value Interval to sleep (ms) between reads
     * @param sessionId Id of the session where the requested originated.
     *        This should be passed with any requests made from this function
     *        to allow for proper state maintenance.
     * @return \c true on successfull set (valid value), \c false otherwise.
     */
    virtual bool setInterval(const unsigned int value, const int sessionId);

    QList<int> sysfsDescriptors_; /**< List of open file descriptors. */
    QMutex mutex_;

private:
    /**
     * Opens all file descriptors required by the adaptor.
     */
    bool openFds();

    /**
     * Closes all file descriptors.
     */
    void closeAllFds();

    /**
     *
     */
    void stopReaderThread();
    bool startReaderThread();

    SysfsAdaptorReader  reader_;

    PollMode            mode_;
    QString             path_;
    int                 epollDescriptor_;
    int                 pipeDescriptors_[2];

    QStringList         paths_;
    QList<int>          pathIds_;

    unsigned int interval_;

    bool initNotDone;
    bool inStandbyMode_;
    bool running_;
    bool shouldBeRunning_;
    bool doSeek_;

    friend class SysfsAdaptorReader;
};

#endif
