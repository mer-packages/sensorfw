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

#ifndef HybrisAdaptor_H
#define HybrisAdaptor_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "deviceadaptor.h"
#include <hardware/sensors.h>

class HybrisAdaptor;

class HybrisAdaptorReader : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(HybrisAdaptorReader)

public:

    HybrisAdaptorReader(QObject *parent);
    ~HybrisAdaptorReader();

    void run();
    void stopReader();
    void startReader();
    void setDevice();

    int sensorHandle;
    int sensorType;

private:
    bool running_;
};


class HybrisManager : public QObject
{
    Q_OBJECT
public:
    explicit HybrisManager(QObject *parent = 0);
    static HybrisManager *instance();
    virtual ~HybrisManager();

    struct sensors_poll_device_t* device;
    struct sensor_t const* sensorList;
    struct sensors_module_t* module;

    int handleForType(int sensorType);
    int maxRange(int sensorType);
    int minDelay(int sensorType);
    int resolution(int sensorType);

    bool setDelay(int handle, int interval);
    void startReader(HybrisAdaptor *adaptor);
    void stopReader(HybrisAdaptor *adaptor);

    bool resumeReader(HybrisAdaptor *adaptor);
    void standbyReader(HybrisAdaptor *adaptor);

    bool openSensors();
    bool closeSensors();

    void registerAdaptor(HybrisAdaptor * adaptor);

    void processSample(const sensors_event_t& data);
    HybrisAdaptorReader adaptorReader;

protected:
    void init();
    int sensorsCount;
    QMap <int, int> sensorMap; //type, index
    QMap <int, HybrisAdaptor *> registeredAdaptors; //type, obj
    bool sensorsOpened;
    void closeAllSensors();
};

class HybrisAdaptor : public DeviceAdaptor
{
public:
    HybrisAdaptor(const QString& id, int type);
    virtual ~HybrisAdaptor();

    virtual void init();
    bool addSensorType(int type);
    virtual bool startAdaptor();
    bool isRunning() const;
    virtual void stopAdaptor();

    virtual bool startSensor();
    virtual void stopSensor();

    virtual bool standby();

    virtual bool resume();

    qreal maxRange;
    qint32 minDelay;
    qreal resolution;

    friend class HybrisManager;
    int sensorHandle;
    int sensorType;
    int cachedInterval;

protected:
    virtual void processSample(const sensors_event_t& data) = 0;

    virtual unsigned int interval() const;
    virtual bool setInterval(const unsigned int value, const int sessionId);
    virtual unsigned int evaluateIntervalRequests(int& sessionId) const;

private:
    void stopReaderThread();
    bool startReaderThread();

    QList<int> sensorIds;
    unsigned int interval_;
    bool inStandbyMode_;
    bool running_;
    bool shouldBeRunning_;

};

#endif // HybrisAdaptor_H
