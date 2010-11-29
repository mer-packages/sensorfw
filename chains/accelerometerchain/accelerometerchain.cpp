/**
   @file accelerometerchain.cpp
   @brief AccelerometerChain

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

#include "accelerometerchain.h"
#include <QVariant>
#include <QStringList>
#include "sensord/sensormanager.h"
#include "sensord/bin.h"
#include "sensord/bufferreader.h"
#include "sensord/config.h"
#include "sensord/logging.h"

#include "coordinatealignfilter/coordinatealignfilter.h"

double AccelerometerChain::aconv_[3][3] = { { 1.0, 0.0, 0.0 },
                                            { 0.0, 1.0, 0.0 },
                                            { 0.0, 0.0, 1.0 } };

AccelerometerChain::AccelerometerChain(const QString& id) :
    AbstractChain(id)
{
    SensorManager& sm = SensorManager::instance();
    
    accelerometerAdaptor_ = sm.requestDeviceAdaptor("accelerometeradaptor");
    Q_ASSERT( accelerometerAdaptor_ );
    if (!accelerometerAdaptor_->isValid()) {
        isValid_ = false;
    } else {
        isValid_ = true;
    }

    accelerometerReader_ = new BufferReader<AccelerationData>(128);

    // Get the transformation matrix from config file
    QString aconvString = Config::configuration()->value("acc_trans_matrix", "").toString();
    if (aconvString.size() > 0)
    {
        if (!setMatrixFromString(aconvString))
        {
            sensordLogW() << "Failed to parse 'acc_trans_matrix' configuration key. Coordinate alignment may be invalid";
        }
    } else {
        sensordLogT() << "Key 'acc_trans_matrix' not found from configuration.";
    }

    accCoordinateAlignFilter_ = sm.instantiateFilter("coordinatealignfilter");
    Q_ASSERT(accCoordinateAlignFilter_);
    qRegisterMetaType<TMatrix>("TMatrix");
    ((CoordinateAlignFilter*)accCoordinateAlignFilter_)->setProperty("transMatrix", QVariant::fromValue(TMatrix(aconv_)));

    outputBuffer_ = new RingBuffer<AccelerationData>(128);
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

// TODO: Solve thread safety for start...
bool AccelerometerChain::start()
{
    if (AbstractSensorChannel::start()) {
        sensordLogD() << "Starting AccelerometerChain";
        filterBin_->start();
        accelerometerAdaptor_->startSensor("accelerometer");
    }
    return true;
}

bool AccelerometerChain::stop()
{
    if (AbstractSensorChannel::stop()) {
        sensordLogD() << "Stopping AccelerometerChain";
        accelerometerAdaptor_->stopSensor("accelerometer");
        filterBin_->stop();
    }
    return true;
}

bool AccelerometerChain::setMatrixFromString(const QString str)
{
    QStringList strList = str.split(',');
    if (strList.size() != 9) {
        sensordLogW() << "Invalid cell count from matrix. Expected 9, got" << strList.size();
        return false;
    }

    for (int i = 0; i < 9; i++)
    {
        aconv_[i/3][i%3] = strList.at(i).toInt();
    }

    return true;
}
