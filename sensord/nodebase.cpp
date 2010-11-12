/**
   @file nodebase.cpp
   @brief Base class for all nodes in filtering chain

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
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

#include "nodebase.h"
#include "logging.h"
#include "ringbuffer.h"

bool NodeBase::isMetadataValid() const
{
    if (!hasLocalRange())
    {
        if (m_dataRangeSource == NULL)
        {
            return false;
        } else {
            return m_dataRangeSource->isMetadataValid();
        }
    }
    return true;
}

QString NodeBase::description() const
{
    return m_description;
}

void NodeBase::setDescription(const QString str)
{
    m_description = str;
}

void NodeBase::introduceAvailableDataRange(const DataRange& range)
{
    if (!m_dataRangeList.contains(range))
    {
        m_dataRangeList.append(range);
    }
}

QList<DataRange> NodeBase::getAvailableDataRanges() const
{
    if (hasLocalRange())
    {
        return m_dataRangeList;
    } else if (m_dataRangeSource != NULL)
    {
        return m_dataRangeSource->getAvailableDataRanges();
    }
    // TODO: Set error
    return QList<DataRange>();
}

DataRangeRequest NodeBase::getCurrentDataRange() const
{
    if (hasLocalRange())
    {
        if (m_dataRangeQueue.empty()) {
            DataRangeRequest rangeRequest;
            rangeRequest.id_ = -1;
            rangeRequest.range_ = m_dataRangeList.at(0);
            return rangeRequest;
        }

        return m_dataRangeQueue.at(0);
    } else if (m_dataRangeSource != NULL)
    {
        return m_dataRangeSource->getCurrentDataRange();
    }
    // TODO: Set error
    return DataRangeRequest();
}

void NodeBase::requestDataRange(int sessionId, DataRange range)
{
    if (hasLocalRange())
    {
        // Do not process invalid ranges
        if (!(m_dataRangeList.contains(range))) {
            return;
        }

        // Check if the range is going to change (no requests or we have the
        // active request)
        bool rangeChanged = false;
        if (m_dataRangeQueue.empty())
        {
            if (!(range == m_dataRangeList.at(0)))
            {
                rangeChanged = true;
            }
        } else {
            if (m_dataRangeQueue.at(0).id_ == sessionId && !(m_dataRangeQueue.at(0).range_ == range)) {
                rangeChanged = true;
            }
        }

        // If an earlier request exists by same id, replace.
        bool hadPreviousRequest = false;
        for (int i = 0; i < m_dataRangeQueue.size() && hadPreviousRequest == false; i++) {
            if (m_dataRangeQueue.at(i).id_ == sessionId) {
                m_dataRangeQueue[i].range_ = range;
                hadPreviousRequest = true;
            }
        }

        if (!hadPreviousRequest) {
            DataRangeRequest request = { sessionId, range };
            m_dataRangeQueue.append(request);
        }


        if (rangeChanged)
        {
            DataRangeRequest currentRequest = getCurrentDataRange();
            if (!setDataRange(currentRequest.range_, currentRequest.id_))
            {
                sensordLogW() << "Failed to set DataRange.";
            }
            emit propertyChanged("datarange");
        }
    } else if (m_dataRangeSource != NULL)
    {
        m_dataRangeSource->requestDataRange(sessionId, range);
    }
}

void NodeBase::removeDataRangeRequest(int sessionId)
{
    if (hasLocalRange())
    {
        int index = -1;
        for (int i = 0; i < m_dataRangeQueue.size() && index == -1; i++) {
            if (m_dataRangeQueue.at(i).id_ == sessionId) {
                index = i;
            }
        }

        if (index < 0) {
            sensordLogD() << "No data range request for id " << sessionId;
            return;
        }

        DataRangeRequest request = m_dataRangeQueue.takeAt(index);

        bool rangeChanged = false;

        if (index == 0)
        {
            if (((m_dataRangeQueue.size() > 0) && !(m_dataRangeQueue.at(0).range_ == request.range_)) ||
                !(m_dataRangeList.at(0) == request.range_))
            {
                rangeChanged = true;
            }
        }

        if (rangeChanged)
        {
            DataRangeRequest currentRequest = getCurrentDataRange();
            if (!setDataRange(currentRequest.range_, currentRequest.id_))
            {
                sensordLogW() << "Failed to set DataRange.";
            }
            emit propertyChanged("datarange");
        }
    } else if (m_dataRangeSource != NULL)
    {
        m_dataRangeSource->removeDataRangeRequest(sessionId);
    }
}
void NodeBase::setRangeSource(NodeBase* node)
{
    m_dataRangeSource = node;
    connect(m_dataRangeSource, SIGNAL(propertyChanged(const QString&)), this, SIGNAL(propertyChanged(const QString&)));
}

bool NodeBase::hasLocalRange() const
{
    return (m_dataRangeSource == NULL);
}

QList<DataRange> NodeBase::getAvailableIntervals() const
{
    if (!hasLocalInterval())
    {
        return m_intervalSource->getAvailableIntervals();
    }

    return m_intervalList;
}

void NodeBase::introduceAvailableInterval(const DataRange& interval)
{
    if (!m_intervalList.contains(interval))
    {
        m_intervalList.append(interval);
    }
}

unsigned int NodeBase::getInterval() const
{
    if (!hasLocalInterval())
    {
        return m_intervalSource->getInterval();
    }

    return interval();
}

bool NodeBase::setIntervalRequest(const int sessionId, const unsigned int value)
{
    // Has single defined source, pass the request that way
    if (!hasLocalInterval())
    {
        return m_intervalSource->setIntervalRequest(sessionId, value);
    }

    // Validate interval request
    if (!isValidIntervalRequest(value))
    {
        sensordLogD() << "Invalid interval requested.";
        return false;
    }

    // Store the request for the session
    m_intervalMap[sessionId] = value;

    // Store the current interval
    unsigned int previousInterval = interval();

    // Re-evaluate
    int winningSessionId;
    unsigned int winningRequest = evaluateIntervalRequests(winningSessionId);

    if (winningSessionId >= 0) {
        setInterval(winningRequest, winningSessionId);
    }

    // Signal listeners about change
    // TODO: Signals should be connected correctly for custom int. handling
    //       make a wrapper for registering custom targets.
    if (previousInterval != interval())
    {
        emit propertyChanged("interval");
    }

    return true;
}

void NodeBase::addStandbyOverrideSource(NodeBase* node)
{
    m_standbySourceList.append(node);
}

bool NodeBase::standbyOverride() const
{
    if (m_standbySourceList.size() == 0) {
        return false;
    }

    bool returnValue = true;
    foreach (NodeBase* node, m_standbySourceList)
    {
        returnValue = returnValue && node->standbyOverride();
    }
    return returnValue;
}

bool NodeBase::setStandbyOverrideRequest(const int sessionId, const bool override)
{
    // Only store true requests, id is enough, no need for value
    if (override == false)
    {
        m_standbyRequestList.removeAll(sessionId);
    } else {
        if (!m_standbyRequestList.contains(sessionId))
        {
            m_standbyRequestList.append(sessionId);
        }
    }

    // Re-evaluate state for nodes that implement handling locally.
    if (m_standbySourceList.size() == 0)
    {
        return setStandbyOverride(m_standbyRequestList.size() > 0);
    }

    // Pass request to sources
    bool returnValue = true;
    foreach (NodeBase* node, m_standbySourceList)
    {
        returnValue = node->setStandbyOverrideRequest(sessionId, override) && returnValue;
    }

    // Revert changes if any source failed while trying to set true.
    if (override == true && returnValue == false)
    {
        foreach (NodeBase* node, m_standbySourceList)
        {
            node->setStandbyOverrideRequest(sessionId, false);
        }
    }

    return returnValue;
}

bool NodeBase::hasLocalInterval() const
{
    return (m_intervalSource == NULL);
}

void NodeBase::setIntervalSource(NodeBase* node)
{
    m_intervalSource = node;
    connect(m_intervalSource, SIGNAL(propertyChanged(const QString&)), this, SIGNAL(propertyChanged(const QString&)));
}

unsigned int NodeBase::evaluateIntervalRequests(int& sessionId) const
{
    unsigned int highestValue = 0;
    int winningSessionId = -1;

    if (m_intervalMap.size() == 0)
    {
        sessionId = winningSessionId;
        return defaultInterval();
    }

    // Get the winning request
    QMap<int, unsigned int>::const_iterator it;
    it = m_intervalMap.begin();
    highestValue = it.value();
    winningSessionId = it.key();

    for (++it; it != m_intervalMap.end(); ++it)
    {
        if (it.value() < highestValue) {
            highestValue = it.value();
            winningSessionId = it.key();
        }
    }

    sessionId = winningSessionId;
    return highestValue;
}

unsigned int NodeBase::defaultInterval() const
{
    return m_defaultInterval;
}

bool NodeBase::setDefaultInterval(const unsigned int value)
{
    if (!isValidIntervalRequest(value))
    {
        sensordLogW() << "Attempting to define invalid default data rate.";
        return false;
    }
    m_defaultInterval = value;
    m_hasDefault = true;
    return true;
}

bool NodeBase::requestDefaultInterval(const int sessionId)
{
    foreach (NodeBase *source, m_sourceList)
    {
        source->requestDefaultInterval(sessionId);
    }

    if (m_hasDefault)
    {
        return setIntervalRequest(sessionId, defaultInterval());
    }
    return true;
}

void NodeBase::removeIntervalRequest(const int sessionId)
{
    unsigned int previousInterval = interval();

    foreach (NodeBase *source, m_sourceList)
    {
        source->removeIntervalRequest(sessionId);
    }

    if (hasLocalInterval())
    {
        // Remove from local list
        if (m_intervalMap.keys().contains(sessionId))
        {
            m_intervalMap.remove(sessionId);
        }

        // Re-evaluate local setting
        int winningSessionId;
        unsigned int winningRequest = evaluateIntervalRequests(winningSessionId);
        if (winningSessionId >= 0) {
            setInterval(winningRequest, winningSessionId);
        }

        // Signal listeners if changed.
        if (previousInterval != interval())
        {
            emit propertyChanged("interval");
        }
    }
}

bool NodeBase::connectToSource(NodeBase *source, const QString bufferName, RingBufferReaderBase *reader)
{
    RingBufferBase* rb;
    rb = source->findBuffer(bufferName);
    if (rb == NULL)
    {
        // This is critical as long as connections are statically defined.
        sensordLogC() << "Buffer '" << bufferName << "' not found while building connections";
        return false;
    }

    bool success = rb->join(reader);

    if (success)
    {
        // Store a reference to the source
        m_sourceList.append(source);
    }

    return success;
}

bool NodeBase::disconnectFromSource(NodeBase *source, const QString bufferName, RingBufferReaderBase *reader)
{
    RingBufferBase* rb;
    rb = source->findBuffer(bufferName);
    if (rb == NULL)
    {
        sensordLogW() << "Buffer '" << bufferName << "' not found while erasing connections";
        return false;
    }

    bool success = rb->unjoin(reader);

    if (success)
    {
        // Remove the source reference from storage
        if (!m_sourceList.removeOne(source))
        {
            sensordLogW() << "Buffer '" << bufferName << "' not disconnected properly.";
        }
    }

    return success;
}

bool NodeBase::isValidIntervalRequest(const unsigned int value) const
{
    for (int i = 0; i < m_intervalList.size(); i++) {
        if (m_intervalList.at(i).min <= value && m_intervalList.at(i).max >= value)
        {
            return true;
        }
    }
    return false;
}

IntegerRangeList NodeBase::getAvailableBufferSizes() const
{
    IntegerRangeList list;
    list.push_back(IntegerRange(1, 200));
    return list;
}
