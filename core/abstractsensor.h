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

#include "nodebase.h"
#include "logging.h"
#include "sfwerror.h"
#include "datarange.h"
#include "genericdata.h"
#include "orientationdata.h"

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

    SensorError errorCode() const { return errorCode_; }
    QString errorString() const { return errorString_; }

    /** Type of the sensor channel */
    const QString type() const { return metaObject()->className(); }

    /** Whether sensor is running or not */
    bool running() { return (cnt_ > 0); }

    void setDownsamplingEnabled(int sessionId, bool value);
    bool downsamplingEnabled(int sessionId);
    virtual bool downsamplingSupported() const;

    virtual void removeSession(int sessionId);

public Q_SLOTS:
    /**
     * @brief Start data flow.
     *
     * This base class implementation is responsible for reference counting.
     * Each child implementation should call the base class method, and verify
     * the return value.
     *
     * Child class implementation is responsible for starting the sensor
     * data flow depending on output from the base class method.
     *
     * @return <b>Base class:</b> \c True when start action should be taken,
     *         \c False if not (i.e already running).<br/>
     *         <b>Child class:</b> \c True on successfull start or already
     *         running, \c False on error.
     */
    virtual bool start();
    bool start(int sessionId);

    /**
     * @brief Stop data flow.
     *
     * This base class implementation is responsible for reference counting.
     * Each child implementation should call the base class method, and verify
     * the return value.
     *
     * Child class implementation is responsible for stopping the sensor
     * data flow depending on output from the base calss method.
     *
     * @return <b>Base class:</b>\c True when start action should be taken,
     *         \c False if not (i.e. Already stopped, or other listeners present).<br/>
     *         <b>Child class:</b>\c True when succesfull, \c False on error.
     */
    virtual bool stop();
    bool stop(int sessionId);

Q_SIGNALS:
    void errorSignal(int error);

protected:
    AbstractSensorChannel(const QString& id);

    void setError(SensorError errorCode, const QString& errorString);
    void clearError() { errorCode_ = SNoError; errorString_.clear(); }

    bool writeToClients(const void* source, int size);

    typedef QMap<int, QList<TimedXyzData> > TimedXyzDownsampleBuffer;
    typedef QMap<int, QList<CalibratedMagneticFieldData> > MagneticFieldDownsampleBuffer;

    bool downsampleAndPropagate(const TimedXyzData& data, TimedXyzDownsampleBuffer& buffer);

    bool downsampleAndPropagate(const CalibratedMagneticFieldData& data, MagneticFieldDownsampleBuffer& buffer);

    void signalPropertyChanged(const QString& name)
    {
        emit propertyChanged(name);
    }

private:
    QString             name_;

    SensorError         errorCode_;
    QString             errorString_;

    int                 cnt_;

    QList<int>          activeSessions_;

    QList<DataRange>    intervalList_;

    QMap<int, bool>     downsampling_;

    bool writeToSession(int sessionId, const void* source, int size);
};

typedef AbstractSensorChannel* (*SensorChannelFactoryMethod)(const QString& id);

#endif // ABSTRACTSENSOR_H
