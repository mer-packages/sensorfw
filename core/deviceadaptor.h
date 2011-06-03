/**
   @file deviceadaptor.h
   @brief Base class for device adaptors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
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
class AdaptedSensorEntry
{
public:
    /**
     * Constructor.
     *
     * @param name Name of the sensor.
     * @param description Description of the sensor.
     * @param buffer Buffer of the sensor.
     */
    AdaptedSensorEntry(const QString& name, const QString& description, RingBufferBase* buffer);

    /**
     * Get name of the sensor.
     *
     * @return name of the sensor.
     */
    const QString& name() const;

    /**
     * Is sensor running.
     *
     * @return is sensor running.
     */
    bool isRunning() const;

    /**
     * Set sensor running state.
     *
     * @param running state.
     */
    void setIsRunning(bool isRunning);

    /**
     * Get sensor reference count.
     *
     * @return reference count.
     */
    int referenceCount() const;

    /**
     * Increment sensor reference count.
     *
     * @return new count.
     */
    int addReference();

    /**
     * Decrement sensor reference count.
     *
     * @return new count.
     */
    int removeReference();

    /**
     * Get output buffer.
     *
     * @return output buffer.
     */
    RingBufferBase* buffer() const;

private:
    QString         name_;        /**< unique name of the sensor */
    QString         description_; /**< decription of the sensor */
    bool            running_;     /**< is sensor running */
    int             count_;       /**< reference count */
    RingBufferBase* buffer_;      /**< sensor output buffer */
};

/**
 * Device adaptor class. Represents single sensor
 */
class DeviceAdaptor : public NodeBase
{
    Q_OBJECT
    Q_DISABLE_COPY(DeviceAdaptor)

public:
    /**
     * Constructor.
     *
     * @param id Sensor ID.
     */
    DeviceAdaptor(const QString& id);

    /**
     * Destructor.
     */
    virtual ~DeviceAdaptor();

    /**
     * Inform adaptor about display state.
     *
     * @param display state.
     */
    void setScreenBlanked(bool status);

    virtual bool setStandbyOverride(bool override);

    AdaptedSensorEntry* getAdaptedSensor() const;

    /**
     * Start sensor.
     *
     * @return was sensor started.
     */
    virtual bool startSensor() = 0;

    /**
     * Stop sensor.
     */
    virtual void stopSensor() = 0;

    virtual void init() = 0;

    /**
     * Device standbyoverride state.
     *
     * @return standbyoverride state.
     */
    bool deviceStandbyOverride() const;

    /**
     * Find buffer of given sensor.
     *
     * @param name Sensor ID.
     * @return buffer.
     */
    virtual RingBufferBase* findBuffer(const QString& name) const;

    /**
     * Start adaptor.
     *
     * @return was adaptor started.
     */
    virtual bool startAdaptor() = 0;

    /**
     * Stop adaptor.
     */
    virtual void stopAdaptor() = 0;

    /**
     * Go into standby mode.
     *
     * @return did we go into standby mode.
     */
    virtual bool standby();

    /**
     * Return from standby mode.
     *
     * @return did we resume from standby mode.
     */
    virtual bool resume();

    const QString& name() { return sensor_.first; }

protected:
    void setAdaptedSensor(const QString& name, const QString& description, RingBufferBase* buffer);

    const QPair<QString, AdaptedSensorEntry*>& sensor() const { return sensor_; }

private:
    void setAdaptedSensor(const QString& name, AdaptedSensorEntry* newAdaptedSensor);

    QPair<QString, AdaptedSensorEntry*> sensor_;
    bool standbyOverride_;                        /**< standby override state */
    bool screenBlanked_;                          /**< is display blanked */
};

/**
 * Factory type for device adaptors
 */
typedef DeviceAdaptor* (*DeviceAdaptorFactoryMethod)(const QString& id);

#endif
