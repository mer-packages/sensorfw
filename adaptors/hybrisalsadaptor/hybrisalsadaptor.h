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

#ifndef HYBRISALSADAPTOR_H
#define HYBRISALSADAPTOR_H
#include "hybrisadaptor.h"

#include <QString>
#include <QStringList>
#include <linux/input.h>
#include "deviceadaptorringbuffer.h"
#include "datatypes/orientationdata.h"
#include <QTime>

class HybrisAlsAdaptor : public HybrisAdaptor
{
    Q_OBJECT

public:
    static DeviceAdaptor* factoryMethod(const QString& id) {
        return new HybrisAlsAdaptor(id);
    }
    HybrisAlsAdaptor(const QString& id);
    ~HybrisAlsAdaptor();

    bool startSensor();
    void stopSensor();

    void sendInitialData();

protected:
    void processSample(const sensors_event_t& data);
    void init();

private:
    DeviceAdaptorRingBuffer<TimedUnsigned>* buffer;
    unsigned lastLightValue;

};
#endif
