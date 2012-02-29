/**
   @file accelerometerchain.cpp
   @brief AccelerometerChain

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#include "accelerometerchain.h"
#include <QVariant>
#include <QStringList>
#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"
#include "config.h"
#include "logging.h"

#include "coordinatealignfilter.h"

AccelerometerChain::AccelerometerChain(const QString& id) :
    AbstractChain(id)
{
    setMatrixFromString("1,0,0,\
                         0,1,0,\
                         0,0,1");
    SensorManager& sm = SensorManager::instance();

    accelerometerAdaptor_ = sm.requestDeviceAdaptor("accelerometeradaptor");
    Q_ASSERT( accelerometerAdaptor_ );
    setValid(accelerometerAdaptor_->isValid());

    accelerometerReader_ = new BufferReader<AccelerationData>(1);

    // Get the transformation matrix from config file
    QString aconvString = Config::configuration()->value<QString>("accelerometer/transformation_matrix", "");
    if (aconvString.size() > 0)
    {
        if (!setMatrixFromString(aconvString))
        {
            sensordLogW() << "Failed to parse 'transformation_matrix' configuration key. Coordinate alignment may be invalid";
        }
    }

    accCoordinateAlignFilter_ = sm.instantiateFilter("coordinatealignfilter");
    Q_ASSERT(accCoordinateAlignFilter_);
    ((CoordinateAlignFilter*)accCoordinateAlignFilter_)->setMatrix(TMatrix(aconv_));

    outputBuffer_ = new RingBuffer<AccelerationData>(1);
    nameOutputBuffer("accelerometer", outputBuffer_);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(accelerometerReader_, "accelerometer");
    filterBin_->add(accCoordinateAlignFilter_, "acccoordinatealigner");
    filterBin_->add(outputBuffer_, "buffer");

    // Join filterchain buffers
    filterBin_->join("accelerometer", "source", "acccoordinatealigner", "sink");
    filterBin_->join("acccoordinatealigner", "source", "buffer", "sink");

    // Join datasources to the chain
    connectToSource(accelerometerAdaptor_, "accelerometer", accelerometerReader_);

    setDescription("Coordinate transformations");
    setRangeSource(accelerometerAdaptor_);
    addStandbyOverrideSource(accelerometerAdaptor_);
    setIntervalSource(accelerometerAdaptor_);
}

AccelerometerChain::~AccelerometerChain()
{
    SensorManager& sm = SensorManager::instance();

    disconnectFromSource(accelerometerAdaptor_, "accelerometer", accelerometerReader_);

    sm.releaseDeviceAdaptor("accelerometeradaptor");

    delete accelerometerReader_;
    delete accCoordinateAlignFilter_;
    delete outputBuffer_;
    delete filterBin_;
}

bool AccelerometerChain::start()
{
    if (AbstractSensorChannel::start()) {
        sensordLogD() << "Starting AccelerometerChain";
        filterBin_->start();
        accelerometerAdaptor_->startSensor();
    }
    return true;
}

bool AccelerometerChain::stop()
{
    if (AbstractSensorChannel::stop()) {
        sensordLogD() << "Stopping AccelerometerChain";
        accelerometerAdaptor_->stopSensor();
        filterBin_->stop();
    }
    return true;
}

bool AccelerometerChain::setMatrixFromString(const QString& str)
{
    QStringList strList = str.split(',');
    if (strList.size() != 9) {
        sensordLogW() << "Invalid cell count from matrix. Expected 9, got" << strList.size();
        return false;
    }

    for (int i = 0; i < 9; ++i)
    {
        aconv_[i/3][i%3] = strList.at(i).toInt();
    }

    return true;
}
