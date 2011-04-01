/**
   @file downsamplefilter.h
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

#ifndef DOWNSAMPLEFILTER_H
#define DOWNSAMPLEFILTER_H

#include <QList>
#include <QObject>
#include "datatypes/orientationdata.h"
#include "filter.h"

/**
 * @brief Downsample filter.
 *
 * Downsamples incoming XYZ data by having defined buffer sizes from where
 * average will be calculated when the buffer is full. Timeout can be used
 * to control how old samples get discarded.
 */
class DownsampleFilter : public QObject, public Filter<TimedXyzData, DownsampleFilter, TimedXyzData>
{
    Q_OBJECT
    Q_DISABLE_COPY(DownsampleFilter)
    Q_PROPERTY(unsigned int bufferSize READ bufferSize WRITE setBufferSize)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)

public:

    /**
     * Factory method.
     *
     * @return New DownsampleFilter instance.
     */
    static FilterBase* factoryMethod() { return new DownsampleFilter; }

    /**
     * Buffer size for downsampling.
     *
     * @return buffer size.
     */
    unsigned int bufferSize() const;

    /**
     * Set buffer size for downsampling.
     *
     * @param size buffer size.
     */
    void setBufferSize(unsigned int size);

    /**
     * Timeout in milliseconds for discarding too old samples from buffer.
     *
     * @return timeout in milliseconds.
     */
    int timeout() const;

    /**
     * Set timeout in milliseconds for discarding too old samples from
     * buffer.
     *
     * @param ms timeout in milliseconds.
     */
    void setTimeout(int ms);

protected:
    /**
     * Constructor.
     */
    DownsampleFilter();

private:
    /**
     * Callback for incoming data to be downsampled.
     */
    void filter(unsigned, const TimedXyzData*);

    /** Sample buffer type for TimedXyzData downsampling. */
    typedef QList<TimedXyzData> TimedXyzDownsampleBuffer;

    unsigned int bufferSize_; /**< buffer size */
    long timeout_;   /**< timeout in milliseconds */
    TimedXyzDownsampleBuffer buffer_; /**< downsample buffer */
};

#endif // DOWNSAMPLEFILTER_H
