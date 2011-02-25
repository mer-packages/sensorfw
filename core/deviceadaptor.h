/**
   @file deviceadaptor.h
   @brief Base class for device adaptors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
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

#ifndef DEVICEADAPTOR_H
#define DEVICEADAPTOR_H

#include <QString>
#include <QHash>
#include <QPair>
#include "logging.h"
#include "nodebase.h"

class RingBufferBase;

/**
 * Represents a single adapted sensor.
 */
class AdaptedSensorEntry : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString description READ description)

    Q_PROPERTY(unsigned interval READ interval WRITE setInterval)

    Q_PROPERTY(bool isRunning READ isRunning WRITE setIsRunning)

public:
    AdaptedSensorEntry(const QString& name, const QString& description, RingBufferBase* buffer)
        : name_(name), description_(description), interval_(0), buffer_(buffer), running_(false), count_(0) {}

    const QString& name() const { return name_; }
    const QString& description() const { return description_; }

    // sensor subsystem "parameters"
    virtual unsigned interval() const { return interval_; }
    virtual void setInterval(unsigned interval) { interval_ = interval; }

    bool isRunning() const { return running_; }
    void setIsRunning(bool isRunning) { running_ = isRunning; }

    RingBufferBase* buffer() { return buffer_; }

    int referenceCount() const { return count_; }
    int addReference() { count_ += 1; return count_; }
    int removeReference() { count_ -= 1; return count_; }

private:
    QString         name_; // note that name is also stored as key of the QHash entry
    QString         description_;

    unsigned        interval_; // perhaps this should be a proxy to the sensor subsystem

    RingBufferBase* buffer_;
    bool            running_;
    int             count_;
};

/**
 * Device adaptor class. Represents single sensor
 */
class DeviceAdaptor : public NodeBase
{
    Q_OBJECT;

    Q_PROPERTY(QString id READ id);
    Q_PROPERTY(bool isValid READ isValid);

public:

    DeviceAdaptor(const QString id) : id_(id), isValid_(true), standbyOverride_(false), screenBlanked(false) {}
    virtual ~DeviceAdaptor();

    void setScreenBlanked(bool status)
    {
        screenBlanked = status;
    }

    virtual bool isValid() const {return isValid_;}

    virtual bool setStandbyOverride(bool override);

    bool deviceStandbyOverride() const { return standbyOverride_; }

    const QString& id() const { return id_; }

    AdaptedSensorEntry* getAdaptedSensor() const;
    virtual RingBufferBase* findBuffer(const QString& name) const;

    virtual bool startSensor() = 0;
    virtual void stopSensor() = 0;

    virtual bool startAdaptor() = 0;
    virtual void stopAdaptor() = 0;

    virtual void init() = 0;

    virtual bool standby() { return false; }
    virtual bool resume() { return false; }

    const QString& name() { return sensor_.first; }

protected:
    void setAdaptedSensor(const QString& name, const QString& description, RingBufferBase* buffer);

    const QPair<QString, AdaptedSensorEntry*>& sensor() const { return sensor_; }

    virtual void setValid(bool valid) { isValid_ = valid; }

private:
    void setAdaptedSensor(const QString& name, AdaptedSensorEntry* newAdaptedSensor);

    const QString id_;
    bool isValid_;
    bool standbyOverride_;
    QPair<QString, AdaptedSensorEntry*> sensor_;
    bool screenBlanked;
};

typedef DeviceAdaptor* (*DeviceAdaptorFactoryMethod)(const QString& id);

#endif
