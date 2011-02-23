/**
   @file samplesensor.cpp
   @brief SampleSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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

#include "samplesensor.h"

#include "sensormanager.h"
#include "bin.h"
#include "bufferreader.h"

SampleSensorChannel::SampleSensorChannel(const QString& id) :
        AbstractSensorChannel(id),
        DataEmitter<TimedUnsigned>(10),
        previousSample_(0, 0)
{
    SensorManager& sm = SensorManager::instance();

    // Get pointer to chain and create a reader for it
    sampleChain_ = sm.requestChain("samplechain");
    chainReader_ = new BufferReader<TimedUnsigned>(128);

    // Create output buffer
    outputBuffer_ = new RingBuffer<TimedUnsigned>(128);

    // Create buffers for filter chain
    filterBin_ = new Bin;
    filterBin_->add(chainReader_, "chain");
    filterBin_->add(outputBuffer_, "output");

    // One could also add filters here just like in chain. Now we just
    // have direct input -> output link.
    filterBin_->join("chain", "source", "output", "sink");

    // Connect the 'sampledata' buffer in chain and reader.
    connectToSource(sampleChain_, "sampledata", chainReader_);

    marshallingBin_ = new Bin;
    marshallingBin_->add(this, "sensorchannel");

    outputBuffer_->join(this);

    // Set MetaData
    setDescription("Funky sample data");
    setRangeSource(sampleChain_);
    addStandbyOverrideSource(sampleChain_);
    setIntervalSource(sampleChain_);
}

SampleSensorChannel::~SampleSensorChannel()
{
    SensorManager& sm = SensorManager::instance();

    // Disconnect reader
    disconnectFromSource(sampleChain_, "accelerometer", chainReader_);

    // Release chain (dont delete!)
    sm.releaseChain("accelerometerchain");

    // ...and delete locally owned things
    delete chainReader_;
    delete outputBuffer_;
    delete marshallingBin_;
    delete filterBin_;
}

// Start bins and sources
bool SampleSensorChannel::start()
{
    sensordLogD() << "Starting SampleSensorChannel";

    if (AbstractSensorChannel::start()) {
        marshallingBin_->start();
        filterBin_->start();
        sampleChain_->start();
    }
    return true;
}

// Stopping is start() in reverse.
bool SampleSensorChannel::stop()
{
    sensordLogD() << "Stopping SampleSensorChannel";

    if (AbstractSensorChannel::stop()) {
        sampleChain_->stop();
        filterBin_->stop();
        marshallingBin_->stop();
    }
    return true;
}

// Store the previous value for use by the accessor function, and
// push data towards clients (the data will be pushed to socket(s) by
// sensormanager).
void SampleSensorChannel::emitData(const TimedUnsigned& value)
{
    previousSample_ = value;
    writeToClients((const void*)(&value), sizeof(TimedUnsigned));
}
