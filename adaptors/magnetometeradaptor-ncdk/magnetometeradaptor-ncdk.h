/**
   @file magnetometeradaptor-ncdk.h
   @brief MagnetometerAdaptor for ncdk

   <p>
   Copyright (C) 2010-2011 Nokia Corporation

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
#ifndef MAGNETOMETERADAPTOR_NCDK_H
#define MAGNETOMETERADAPTOR_NCDK_H

#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/genericdata.h"
#include <QString>


class MagnetometerAdaptorNCDK : public SysfsAdaptor
{
    Q_OBJECT;
public:

    Q_PROPERTY(int overflowLimit READ overflowLimit WRITE setOverflowLimit);

    /**
     * Factory method for gaining a new instance of MagnetometerAdaptor class.
     * @param id Identifier for the adaptor.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new MagnetometerAdaptorNCDK(id);
    }

    bool startSensor();
    void stopSensor();

protected:
    /**
     * Constructor.
     * @param id Identifier for the adaptor.
     */
    MagnetometerAdaptorNCDK(const QString& id);
    ~MagnetometerAdaptorNCDK();

    bool setInterval(const unsigned int value, const int sessionId);

private:

    /**
     * Read and process data. Run when sysfsadaptor has detected new available
     * data.
     * @param pathId PathId for the file that had event. Always 0, as we monitor
     *               only single file and don't set any proper id.
     * @param fd     Open file descriptor with new data. See #SysfsAdaptor::processSample()
     */
    void processSample(int pathId, int fd);

    QByteArray powerStateFilePath_;
    QByteArray sensAdjFilePath_;

    int x_adj, y_adj, z_adj;
    bool powerState_;
    DeviceAdaptorRingBuffer<TimedXyzData>* magnetometerBuffer_;

    bool setPowerState(bool value) const;
    void getSensitivityAdjustment(int &x, int &y, int &z) const;
    int adjustPos(const int value, const int adj) const;
    int intervalCompensation_;
    int overflowLimit_;

    /**
     * Sets the overflow limit of the sensor, checked when calibrated
     *
     * @param limit overflow limit.
     */
    void setOverflowLimit(int limit);

    /**
     * Get the overflow limit.
     *
     * @return overflow limit.
     */
    int overflowLimit() const;
};

#endif // MAGNETOMETERADAPTOR_NCDK_H
