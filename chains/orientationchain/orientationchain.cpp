/**
   @file orientationchain.cpp
   @brief OrientationChain

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Üstün Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

#include "orientationchain.h"
#include "sensord/sensormanager.h"
#include "sensord/bin.h"
#include "sensord/bufferreader.h"

OrientationChain::OrientationChain(const QString& id) :
    AbstractChain(id)
{
    qRegisterMetaType<TimedUnsigned>("TimedUnsigned");
    qRegisterMetaType<PoseData>("PoseData");
    SensorManager& sm = SensorManager::instance();

    accelerometerChain_ = sm.requestChain("accelerometerchain");
    Q_ASSERT( accelerometerChain_ );
    if (!accelerometerChain_->isValid()) {
        isValid_ = false;
    } else {
        isValid_ = true;
    }

    accelerometerReader_ = new BufferReader<AccelerationData>(1024);

    orientationInterpreterFilter_ = sm.instantiateFilter("orientationinterpreter");

    topEdgeOutput_ = new RingBuffer<PoseData>(1024);
    nameOutputBuffer("topedge", topEdgeOutput_);

    faceOutput_ = new RingBuffer<PoseData>(1024);
    nameOutputBuffer("face", faceOutput_);

    orientationOutput_ = new RingBuffer<PoseData>(1024);
    nameOutputBuffer("orientation", orientationOutput_);

    // Create buffers for filter chain
    filterBin_ = new Bin;

    filterBin_->add(accelerometerReader_, "accelerometer");
    filterBin_->add(orientationInterpreterFilter_, "orientationinterpreter");
    filterBin_->add(topEdgeOutput_, "topedgebuffer");
    filterBin_->add(faceOutput_, "facebuffer");
    filterBin_->add(orientationOutput_, "orientationbuffer");

    // Join filterchain buffers
    filterBin_->join("accelerometer", "source", "orientationinterpreter", "accsink");
    filterBin_->join("orientationinterpreter", "topedge", "topedgebuffer", "sink");
    filterBin_->join("orientationinterpreter", "face", "facebuffer", "sink");
    filterBin_->join("orientationinterpreter", "orientation", "orientationbuffer", "sink");

    // Join datasources to the chain
    RingBufferBase* rb;
    rb = accelerometerChain_->findBuffer("accelerometer");
    Q_ASSERT(rb);
    rb->join(accelerometerReader_);

    introduceAvailableDataRange(DataRange(0, 6, 1));

    addStandbyOverrideSource(accelerometerChain_);
}

OrientationChain::~OrientationChain()
{
    RingBufferBase* rb;
    rb = accelerometerChain_->findBuffer("accelerometer");
    Q_ASSERT(rb);
    rb->unjoin(accelerometerReader_);

    delete accelerometerReader_;
    delete topEdgeOutput_;
    delete faceOutput_;
    delete filterBin_;
}

// TODO: Solve thread safety for start...
bool OrientationChain::start()
{
    if (AbstractSensorChannel::start()) {
        sensordLogD() << "Starting AccelerometerChain";
        filterBin_->start();
        accelerometerChain_->start();
    }
    return true;
}

bool OrientationChain::stop()
{
    if (AbstractSensorChannel::stop()) {
        sensordLogD() << "Stopping AccelerometerChain";
        accelerometerChain_->stop();
        filterBin_->stop();
    }
    return true;
}
