/**
   @file samplechain.cpp
   @brief SampleChain

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

#include "samplechain.h"
#include "sensord/sensormanager.h"
#include "sensord/bin.h"
#include "sensord/bufferreader.h"
#include "sensord/config.h"
#include "sensord/logging.h"

SampleChain::SampleChain(const QString& id) :
    AbstractChain(id)
{
    SensorManager& sm = SensorManager::instance();

    // Get a pointer (refcounted) to the adaptor from sensor manager
    sampleAdaptor_ = sm.requestDeviceAdaptor("sampleadaptor");

    // Create a reader for the adaptor
    adaptorReader_ = new BufferReader<TimedUnsigned>(16);

    // Get an instance of filter from sensor manager
    sampleFilter_ = sm.instantiateFilter("samplefilter");

    // Create and name output buffer. This name is used by other nodes
    // to find the buffer.
    outputBuffer_ = new RingBuffer<TimedUnsigned>(16);
    nameOutputBuffer("sampledata", outputBuffer_);

    // Create a new bin and add elements into it with names.
    filterBin_ = new Bin;
    filterBin_->add(adaptorReader_, "adaptor");
    filterBin_->add(sampleFilter_, "filter");
    filterBin_->add(outputBuffer_, "output");

    // Make connections between the elements in the Bin. By default,
    // readers, ringbuffers and simple filters use source and sink
    // as buffer names.
    filterBin_->join("adaptor", "source", "filter", "sink");
    filterBin_->join("filter", "source", "output", "sink");

    // Connect the reader to the adaptor, specifically to buffer named
    // 'sample'
    connectToSource(sampleAdaptor_, "sample", adaptorReader_);

    // Setup metadata. A description is for each component, but as this
    // chain just modifies each sample coming from the adaptor, we can
    // push the responsibility for interval, datarange and such to the
    // adaptor.
    setDescription("Sample filtering with a chain");
    setRangeSource(sampleAdaptor_);
    setIntervalSource(sampleAdaptor_);

    // Adaptors are normally stopped when device screen blanks. This
    // function is used to signify which sources should get the request
    // for overriding this behavior.
    addStandbyOverrideSource(sampleAdaptor_);
}

SampleChain::~SampleChain()
{
    SensorManager& sm = SensorManager::instance();

    // Disconnect the reader from the adaptor
    disconnectFromSource(sampleAdaptor_, "sample", adaptorReader_);

    // Release the adaptor (must not delete, these are shared)
    sm.releaseDeviceAdaptor("sampleadaptor");

    // ..and delete all locally owned stuff.
    delete adaptorReader_;
    delete sampleFilter_;
    delete outputBuffer_;
    delete filterBin_;
}

// In chains one must start the sources and internal bins.
bool SampleChain::start()
{
    if (AbstractSensorChannel::start()) {
        sensordLogD() << "Starting SampleChain";
        filterBin_->start();

        // Adaptors are started on buffer basis, thus the buffer name
        sampleAdaptor_->startSensor("sample");
    }
    return true;
}

// Stopping is pretty much start() in reverse.
bool SampleChain::stop()
{
    if (AbstractSensorChannel::stop()) {
        sensordLogD() << "Stopping SampleChain";
        sampleAdaptor_->stopSensor("sample");
        filterBin_->stop();
    }
    return true;
}
