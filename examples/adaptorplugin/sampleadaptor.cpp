/**
   @file sampleadaptor.cpp
   @brief SampleAdaptor based on SysfsAdaptor

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

#include "sampleadaptor.h"

#include "logging.h"       // Logging entries
#include "config.h"        // Configuration file access
#include "filters/utils.h" // Provides timestamp utility

// Constructor should initialise SysfsAdaptor parent.
// id should be passed directly. See #SysfsAdaptor for details on
// SelectMode. Adding the path at this stage is optional. If added with
// the constructor, it will receive index 0. The path can be fetched
// from configuration (like here). If locating the path needs processing
// the path should be omitted here and set inside the constructor.
SampleAdaptor::SampleAdaptor(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::SelectMode, Config::configuration()->value("sample_dev_path").toString())
{
    // One can use SysfsAdaptor::addPath() to add additional paths for
    // monitoring. The paths are indexed in the order they are
    // added.

    // Initialise the output buffer with reasonable size and type
    outputBuffer_ = new DeviceAdaptorRingBuffer<TimedUnsigned>(64);

    // Provide the output buffer with name and description. Name is used
    // to locate the buffer from the listening side, description is
    // currently unused.
    addAdaptedSensor("sample", "Sample sensor, with dummy buffer", outputBuffer_);

    // Provide information on the data range this adaptor can output
    // (based on the adapted sensor naturally). Parameters of DataRange
    // are min, max, accuracy in respective order.
    //
    // If more then one data range is introduced, the adaptor must also
    // implement #NodeBase::setDataRange() to allow switching between
    // them (not properly tested, as such case has not yet occurred).
    introduceAvailableDataRange(DataRange(0, 65535, 1));
}

SampleAdaptor::~SampleAdaptor()
{
    delete outputBuffer_;
}

void SampleAdaptor::processSample(int pathId, int fd)
{
    // PathID (index of the path, depends on adding order). If only a
    // a single file is monitored, can be ignored. If several files are
    // monitored, can be used to detect which one should be read.
    Q_UNUSED(pathId);

    // Do whatever is needed to get values from the provided file
    // descriptor (fd). Note that one should not manipulate the
    // fd in any other way apart from reading. open/close/seek are taken
    // care of by SysfsAdaptor.
    Q_UNUSED(fd);

    // It's a good thing to provide read values to log for testing
    // level.
    sensordLogT() << "Sample value: "; // << values read from fd

    // Get a pointer to the output buffer
    TimedUnsigned* slot = outputBuffer_->nextSlot();

    // Copy the value to output buffer
    slot->value_ = 0; // value read from fd

    // Set the timestamp for sample
    slot->timestamp_ = Utils::getTimeStamp();

    // Commit the change to buffer, and wakeup any possible listeners.
    outputBuffer_->commit();
    outputBuffer_->wakeUpReaders();
}
