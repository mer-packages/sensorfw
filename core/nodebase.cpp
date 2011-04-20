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
#include "config.h"

NodeBase::NodeBase(const QString& id, QObject* parent) :
    QObject(parent),
    m_bufferSize(0),
    m_bufferInterval(0),
    m_dataRangeSource(NULL),
    m_intervalSource(NULL),
    m_hasDefault(false),
    m_defaultInterval(0),
    DEFAULT_DATA_RANGE_REQUEST(-1),
    id_(id),
    isValid_(false)
{
}

NodeBase::~NodeBase()
{
}

const QString& NodeBase::id() const
{
    return id_;
}

bool NodeBase::isValid() const
{
    return isValid_;
}

bool NodeBase::isMetadataValid() const
{
    if (!hasLocalRange())
    {
        return m_dataRangeSource->isMetadataValid();
    }
    return true;
}

const QString& NodeBase::description() const
{
    return m_description;
}

void NodeBase::setDescription(const QString& str)
{
    m_description = str;
}

void NodeBase::introduceAvailableDataRange(const DataRange& range)
{
    if (!m_dataRangeList.contains(range))
    {
        sensordLogD() << "Introduced new data range: " << range.min << "-" << range.max << ", " << range.resolution;
        m_dataRangeList.append(range);
    }
}

void NodeBase::introduceAvailableDataRanges(const QString& typeName)
{
    QVariant ranges = Config::configuration()->value(typeName + "/dataranges");
    if(ranges.isValid())
    {
        DataRangeList list(parseDataRangeList(ranges.toString(), 1));
        foreach(const DataRange& range, list)
        {
            introduceAvailableDataRange(range);
        }
    }
}

const QList<DataRange>& NodeBase::getAvailableDataRanges() const
{
    if (hasLocalRange())
    {
        return m_dataRangeList;
    }
    else
    {
        return m_dataRangeSource->getAvailableDataRanges();
    }
}

DataRangeRequest NodeBase::getCurrentDataRange() const
{
    if (hasLocalRange()) {
        if (m_dataRangeQueue.empty()) {
            return DataRangeRequest(-1, m_dataRangeList.at(0));
        }
        return m_dataRangeQueue.at(0);
    } else {
        return m_dataRangeSource->getCurrentDataRange();
    }
}

void NodeBase::requestDataRange(int sessionId, const DataRange& range)
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
            if (m_dataRangeQueue.at(0).id == sessionId && !(m_dataRangeQueue.at(0).range == range)) {
                rangeChanged = true;
            }
        }

        // If an earlier request exists by same id, replace.
        bool hadPreviousRequest = false;
        for (int i = 0; i < m_dataRangeQueue.size() && hadPreviousRequest == false; ++i) {
            if (m_dataRangeQueue.at(i).id == sessionId) {
                m_dataRangeQueue[i].range = range;
                hadPreviousRequest = true;
            }
        }

        if (!hadPreviousRequest) {
            DataRangeRequest request(sessionId, range);
            m_dataRangeQueue.append(request);
        }

        if (rangeChanged)
        {
            DataRangeRequest currentRequest = getCurrentDataRange();
            if (!setDataRange(currentRequest.range, currentRequest.id))
            {
                sensordLogW() << "Failed to set DataRange.";
            }
            emit propertyChanged("datarange");
        }
    } else {
        m_dataRangeSource->requestDataRange(sessionId, range);
    }
}

void NodeBase::removeDataRangeRequest(int sessionId)
{
    if (hasLocalRange())
    {
        int index = -1;
        for (int i = 0; i < m_dataRangeQueue.size() && index == -1; ++i) {
            if (m_dataRangeQueue.at(i).id == sessionId) {
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
            if (((m_dataRangeQueue.size() > 0) && !(m_dataRangeQueue.at(0).range == request.range)) ||
                !(m_dataRangeList.at(0) == request.range))
            {
                rangeChanged = true;
            }
        }

        if (rangeChanged)
        {
            DataRangeRequest currentRequest = getCurrentDataRange();
            if (!setDataRange(currentRequest.range, currentRequest.id))
            {
                sensordLogW() << "Failed to set DataRange.";
            }
            emit propertyChanged("datarange");
        }
    } else {
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

DataRangeList NodeBase::parseDataRangeList(const QString& input, int defaultResolution) const
{
    DataRangeList list;
    foreach(const QString& fragment, input.split(",", QString::SkipEmptyParts))
    {
        QStringList pair(fragment.split("=>"));
        QStringList pair2(fragment.split(":"));
        DataRange range;
        range.resolution = defaultResolution;
        if(pair.size() == 1)
        {
            QVariant value(fragment);
            range.min = value.toDouble();
            range.max = range.min;
        }
        else if(pair.size() == 2)
        {
            QVariant value(fragment);
            range.min = QVariant(pair.at(0)).toDouble();
            range.max = QVariant(pair.at(1)).toDouble();
        }
        else
        {
            continue;
        }
        if(pair2.size() == 2)
            range.resolution = QVariant(pair2.at(1)).toDouble();
        list.push_back(range);
    }
    return list;
}

const QList<DataRange>& NodeBase::getAvailableIntervals() const
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
        sensordLogD() << "Introduced new interval: " << interval.min << "-" << interval.max;
        m_intervalList.append(interval);
    }
}

void NodeBase::introduceAvailableIntervals(const QString& typeName)
{
    QVariant ranges = Config::configuration()->value(typeName + "/intervals");
    if(ranges.isValid())
    {
        DataRangeList list(parseDataRangeList(ranges.toString(), 0));
        foreach(const DataRange& range, list)
        {
            introduceAvailableInterval(range);
        }
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

unsigned int NodeBase::getInterval(int sessionId) const
{
    if (!hasLocalInterval())
    {
        return m_intervalSource->getInterval(sessionId);
    }
    QMap<int, unsigned int>::const_iterator it(m_intervalMap.find(sessionId));
    if(it == m_intervalMap.end())
    {
        return 0;
    }
    return it.value();
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
        sensordLogW() << "Invalid interval requested for node '" << id() << "' by session '" << sessionId << "': " << value;
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
        sensordLogD() << "Setting new interval for node: " << id() << ". Evaluation won by session '" << winningSessionId << "' with request: " << winningRequest;
        setInterval(winningRequest, winningSessionId);
    }

    // Signal listeners about change
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
    sensordLogD() << sessionId << " requested standbyoverride for '" << id() << "' :" << override;
    // Only store true requests, id is enough, no need for value
    if (override == false)
    {
        m_standbyRequestList.removeAll(sessionId);
    }
    else
    {
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
    if (m_intervalMap.size() == 0)
    {
        sessionId = -1;
        return defaultInterval();
    }

    // Get the winning request
    QMap<int, unsigned int>::const_iterator it = m_intervalMap.constBegin();
    unsigned int highestValue = it.value();
    int winningSessionId = it.key();

    for (++it; it != m_intervalMap.constEnd(); ++it)
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
        sensordLogW() << "Attempting to define invalid default data rate: " << value;
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

bool NodeBase::connectToSource(NodeBase* source, const QString& bufferName, RingBufferReaderBase* reader)
{
    RingBufferBase* rb = source->findBuffer(bufferName);
    if (rb == NULL)
    {
        // This is critical as long as connections are statically defined.
        sensordLogC() << "Buffer '" << bufferName << "' not found while building connections for node: " << id();
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

bool NodeBase::disconnectFromSource(NodeBase* source, const QString& bufferName, RingBufferReaderBase* reader)
{
    RingBufferBase* rb = source->findBuffer(bufferName);
    if (rb == NULL)
    {
        sensordLogW() << "Buffer '" << bufferName << "' not found while erasing connections for node: " << id();
        return false;
    }

    bool success = rb->unjoin(reader);

    if (success)
    {
        // Remove the source reference from storage
        if (!m_sourceList.removeOne(source))
        {
            sensordLogW() << "Buffer '" << bufferName << "' not disconnected properly for node: " << id();
        }
    }

    return success;
}

bool NodeBase::isValidIntervalRequest(const unsigned int value) const
{
    for(QList<DataRange>::const_iterator it = m_intervalList.constBegin(); it != m_intervalList.constEnd(); ++it)
    {
        if (it->min <= value && it->max >= value)
        {
            return true;
        }
    }
    return false;
}

IntegerRangeList NodeBase::getAvailableBufferSizes(bool& hwSupported) const
{
    IntegerRangeList list;
    foreach (NodeBase* source, m_sourceList)
    {
        list = source->getAvailableBufferSizes(hwSupported);
        if(hwSupported)
            return list;
    }
    if(list.isEmpty())
        list.push_back(IntegerRange(1, 256));
    hwSupported = false;
    return list;
}

IntegerRangeList NodeBase::getAvailableBufferIntervals(bool& hwSupported) const
{
    IntegerRangeList list;
    foreach (NodeBase* source, m_sourceList)
    {
        list = source->getAvailableBufferIntervals(hwSupported);
        if(hwSupported)
            return list;
    }
    if(list.isEmpty())
        list.push_back(IntegerRange(0, 60000));
    hwSupported = false;
    return list;
}

bool NodeBase::setBufferSize(int sessionId, unsigned int value)
{
    bool hwbuffering = false;
    if(!isInRange(value, getAvailableBufferSizes(hwbuffering)))
        return false;
    m_bufferSizeMap.insert(sessionId, value);
    return updateBufferSize();
}

bool NodeBase::clearBufferSize(int sessionId)
{
    int index = m_bufferSizeMap.remove(sessionId);
    updateBufferSize();
    return index != 0;
}

bool NodeBase::updateBufferSize()
{
    int key = 0;
    int value = 0;
    for(QMap<int, unsigned int>::const_iterator it = m_bufferSizeMap.constBegin(); it != m_bufferSizeMap.constEnd(); ++it)
    {
        if(it.key() >= key)
        {
            key = it.key();
            value = it.value();
        }
    }
    if(setBufferSize(value))
    {
        emit propertyChanged("buffersize");
        return true;
    }
    return false;
}

bool NodeBase::setBufferInterval(int sessionId, unsigned int value)
{
    bool hwbuffering = false;
    if(!isInRange(value, getAvailableBufferIntervals(hwbuffering)))
        return false;
    m_bufferIntervalMap.insert(sessionId, value);
    return updateBufferInterval();
}

bool NodeBase::clearBufferInterval(int sessionId)
{
    int index = m_bufferIntervalMap.remove(sessionId);
    updateBufferInterval();
    return index != 0;
}

bool NodeBase::updateBufferInterval()
{
    int key = 0;
    int value = 0;
    for(QMap<int, unsigned int>::const_iterator it = m_bufferIntervalMap.constBegin(); it != m_bufferIntervalMap.constEnd(); ++it)
    {
        if(it.key() >= key)
        {
            key = it.key();
            value = it.value();
        }
    }
    if(setBufferInterval(value))
    {
        emit propertyChanged("bufferinterval");
        return true;
    }
    return false;
}

bool NodeBase::setDataRangeIndex(int sessionId, int rangeIndex)
{
    if (rangeIndex < 0)
        return false;
    if (rangeIndex > (m_dataRangeList.size() - 1))
        return false;
    requestDataRange(sessionId, m_dataRangeList.at(rangeIndex));
    DataRangeList ranges = getAvailableDataRanges();
    DataRange range = getCurrentDataRange().range;
    return ranges.at(rangeIndex) == range;
}

void NodeBase::removeSession(int sessionId)
{
    setStandbyOverrideRequest(sessionId, false);
    removeIntervalRequest(sessionId);
    removeDataRangeRequest(sessionId);
    clearBufferSize(sessionId);
    clearBufferInterval(sessionId);
}

void NodeBase::setValid(bool valid)
{
    isValid_ = valid;
    if(!isValid_)
    {
        sensordLogW() << "Node '" << id() << "' state changed to invalid";
    }
}

bool NodeBase::setDataRange(const DataRange& range, int sessionId)
{
    Q_UNUSED(range);
    Q_UNUSED(sessionId);
    return false;
}

bool NodeBase::setStandbyOverride(bool override)
{
    Q_UNUSED(override);
    return false;
}

unsigned int NodeBase::interval() const
{
    return 0;
}

bool NodeBase::setInterval(unsigned int value, int sessionId)
{
    sensordLogW() << "setInterval() not implemented in some node using it.";
    Q_UNUSED(value);
    Q_UNUSED(sessionId);
    return false;
}

bool NodeBase::setBufferSize(unsigned int value)
{
    Q_UNUSED(value);
    return false;
}

bool NodeBase::setBufferInterval(unsigned int value)
{
    Q_UNUSED(value);
    return false;
}
