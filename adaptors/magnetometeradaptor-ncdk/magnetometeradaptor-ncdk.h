/**
   @file magnetometeradaptor-ncdk.h
   @brief MagnetometerAdaptor for ncdk

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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
#ifndef MAGNETOMETERADAPTOR_NCDK_H
#define MAGNETOMETERADAPTOR_NCDK_H

#include "sysfsadaptor.h"
#include "deviceadaptorringbuffer.h"
#include "datatypes/genericdata.h"
#include <QString>


class MagnetometerAdaptorNCDK : public SysfsAdaptor
{
public:

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

    QString powerStateFilePath_;
    QString sensAdjFilePath_;
    QString sensAdj;
    int x_adj, y_adj, z_adj;
    DeviceAdaptorRingBuffer<TimedXyzData>* magnetometerBuffer_;

    int PoserState() ;
    bool setPowerState(const int value);
    QString sensAdjust();
    int adjustPos(const int value, const int adj) const;
    int intervalCompensation_;
};

#endif // MAGNETOMETERADAPTOR_NCDK_H
