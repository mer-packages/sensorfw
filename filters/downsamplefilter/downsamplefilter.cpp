/**
   @file downsamplefilter.cpp
   @brief DownsampleFilter

   <p>
   Copyright (C) 2011 Nokia Corporation

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

#include "downsamplefilter.h"
#include "logging.h"

DownsampleFilter::DownsampleFilter() :
    Filter<TimedXyzData, DownsampleFilter, TimedXyzData>(this, &DownsampleFilter::filter),
    bufferSize_(1),
    timeout_(-1)
{
}

unsigned int DownsampleFilter::bufferSize() const
{
    return bufferSize_;
}

void DownsampleFilter::setBufferSize(unsigned int size)
{
    sensordLogD() << "DownsampleFilter buffer size = " << size;
    bufferSize_ = size;
}

int DownsampleFilter::timeout() const
{
    return timeout_ / 1000;
}

void DownsampleFilter::setTimeout(int ms)
{
    timeout_ = static_cast<long>(ms) * 1000;
    sensordLogD() << "DownsampleFilter timeout = " << ms;
}

void DownsampleFilter::filter(unsigned, const TimedXyzData* data)
{
    buffer_.push_back(*data);

    for(TimedXyzDownsampleBuffer::iterator it = buffer_.begin(); it != buffer_.end(); ++it)
    {
        if(static_cast<unsigned int>(buffer_.size()) > bufferSize_ ||
           (timeout_ && (data->timestamp_ - it->timestamp_ >
                         static_cast<unsigned long>(timeout_))))
        {
            it = buffer_.erase(it);
            if(it == buffer_.end())
                break;
        }
        else
            break;
    }

    if(static_cast<unsigned int>(buffer_.size()) < bufferSize_)
        return;

    long x = 0;
    long y = 0;
    long z = 0;
    foreach(const TimedXyzData& data, buffer_)
    {
        x += data.x_;
        y += data.y_;
        z += data.z_;
    }
    int count = buffer_.count();
    TimedXyzData downsampled(data->timestamp_,
                             x / count,
                             y / count,
                             z / count);

    sensordLogT() << "Downsampled: " << downsampled.x_ << ", " << downsampled.y_ << ", " << downsampled.z_;

    source_.propagate(1, &downsampled);
    buffer_.clear();
}
