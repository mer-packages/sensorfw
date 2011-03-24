/**
   @file abstractsensor.h
   @brief Base class for sensors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>

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

#ifndef ABSTRACTSENSOR_H
#define ABSTRACTSENSOR_H

#include <QString>
#include <QMap>
#include <QList>
#include <QSet>

#include "nodebase.h"
#include "logging.h"
#include "sfwerror.h"
#include "datarange.h"
#include "genericdata.h"
#include "orientationdata.h"

/**
 * Base class for sensor type specific nodes. This is used as base class
 * for chains and graph endpoint nodes which are responsible of streaming
 * data to the sessions.
 */
class AbstractSensorChannel : public NodeBase
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractSensorChannel)
    Q_PROPERTY(SensorError errorCode READ errorCode)
    Q_PROPERTY(QString errorString READ errorString)
    Q_PROPERTY(QString type READ type)
    Q_PROPERTY(bool running READ running)

public:
    /**
     * Destructor.
     */
    virtual ~AbstractSensorChannel() {}

    /**
     * Last occured error.
     *
     * @return last occured error.
     */
    SensorError errorCode() const;

    /**
     * Description of last occured error.
     *
     * @return description of last occured error.
     */
    const QString& errorString() const;

    /**
     * Class name of the sensor channel.
     *
     * @return Class name of sensor channel.
     */
    const QString type() const;

    /**
     * Is sensor running or not. Sensor is running if there are any
     * open sessions for it.
     *
     * @return is sensor running.
     */
    bool running() const;

    /**
     * Enable or disable downsampling for given session.
     *
     * @param sessionId session ID.
     * @param value downsampling state.
     */
    void setDownsamplingEnabled(int sessionId, bool value);

    /**
     * Is downsampling enabled for given session.
     *
     * @param sessionId session ID.
     * @return is downsampling enabled.
     */
    bool downsamplingEnabled(int sessionId) const;

    /**
     * Is downsampling supported for this object.
     *
     * @return is downsampling supported.
     */
    virtual bool downsamplingSupported() const;

    virtual void removeSession(int sessionId);

    /**
     * Start data flow. Base class implementation is responsible for
     * reference counting. Which each subclass is responsible of calling.
     * Subclass implementation is responsible for starting the sensor
     * data flow depending on output from the base class method.
     *
     * @return <b>Base class:</b> \c True when start action should be taken,
     *         \c False if not (i.e already running).<br/>
     *         <b>Child class:</b> \c True on successful start or already
     *         running, \c False on error.
     */
    virtual bool start();

    /**
     * Start data flow for given session.
     *
     * @param sessionId session ID.
     * @return True if sensor was started. False if it is already running.
     */
    bool start(int sessionId);

    /**
     * Stop data flow. Base class implementation is responsible for
     * reference counting. Subclass implementation is responsible of
     * stopping the sensor data flow depending on output from the base class
     * method.
     *
     * @return <b>Base class:</b>\c True when start action should be taken,
     *         \c False if not (i.e. Already stopped, or other listeners present).<br/>
     *         <b>Child class:</b>\c True when succesfull, \c False on error.
     */
    virtual bool stop();

    /**
     * Stop data flow for given session.
     *
     * @param sessionId session ID.
     * @return True if sensor was stopped. False if it is already stopped.
     */
    bool stop(int sessionId);

Q_SIGNALS:
    /**
     * Signal is emitted for occured errors.
     *
     * @param error Error number.
     */
    void errorSignal(int error);

protected:
    /** Sample buffer type for TimedXyzData downsampling. */
    typedef QMap<int, QList<TimedXyzData> > TimedXyzDownsampleBuffer;

    /** Sample buffer type for CalibratedMagneticFieldData downsampling. */
    typedef QMap<int, QList<CalibratedMagneticFieldData> > MagneticFieldDownsampleBuffer;

    /**
     * Constructor.
     *
     * @param id Sensor ID.
     */
    AbstractSensorChannel(const QString& id);

    /**
     * Set error information.
     *
     * @param errorCode error code.
     * @param errorString error description.
     */
    void setError(SensorError errorCode, const QString& errorString);

    /**
     * Clear error information.
     */
    void clearError();

    /**
     * Write output data to all connected sessions.
     *
     * @param source Object to write.
     * @param size Size of the object.
     * @return was data succesfully written.
     */
    bool writeToClients(const void* source, int size);

    /**
     * Downsample and propagate data to all connected sessions.
     *
     * @param data Object to handle.
     * @param buffer Data buffer.
     * @return was data succesfully handled.
     */
    bool downsampleAndPropagate(const TimedXyzData& data, TimedXyzDownsampleBuffer& buffer);

    /**
     * Downsample and propagate data to all connected sessions.
     *
     * @param data Object to handle.
     * @param buffer Data buffer.
     * @return was data succesfully handled.
     */
    bool downsampleAndPropagate(const CalibratedMagneticFieldData& data, MagneticFieldDownsampleBuffer& buffer);

    /**
     * Signal property change.
     *
     * @param name property name.
     */
    void signalPropertyChanged(const QString& name);

private:
    /**
     * Write to given session.
     *
     * @param sessionId session ID.
     * @param source source object.
     * @param size size of object to write.
     * @return was data succesfully written.
     */
    bool writeToSession(int sessionId, const void* source, int size);

    SensorError         errorCode_;       /**< previous occured error code */
    QString             errorString_;     /**< previous occured error description */
    int                 cnt_;             /**< usage reference count */
    QSet<int>           activeSessions_;  /**< active sessions */
    QMap<int, bool>     downsampling_;    /**< downsample state for sessions */
};

/**
 * Factory type for constructing sensor channel.
 */
typedef AbstractSensorChannel* (*SensorChannelFactoryMethod)(const QString& id);

#endif // ABSTRACTSENSOR_H
