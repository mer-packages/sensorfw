/**
   @file nodebase.cpp
   @brief Base class for all nodes in filtering chain

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

#include <QMutexLocker>

#include "nodebase.h"
#include "logging.h"

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

DataRange NodeBase::getCurrentDataRange() const
{
    if (hasLocalRange())
    {
        if (m_dataRangeQueue.empty()) {
            return m_dataRangeList.at(0);
        }

        return m_dataRangeQueue.at(0).range_;
    } else if (m_dataRangeSource != NULL)
    {
        return m_dataRangeSource->getCurrentDataRange();
    }
    // TODO: Set error
    return DataRange();
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
            if (!setDataRange(getCurrentDataRange()))
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

        if (rangeChanged) {

            if (!setDataRange(getCurrentDataRange()))
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
    if (m_dataRangeList.size() > 0)
    {
        return true;
    }
    return false;
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
    // TODO: validate the need for this
    QMutexLocker locker(&m_mutex);

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

    bool newValue = m_standbyRequestList.size() > 0;
    bool returnValue = true;

    // Implemented locally?
    if (m_standbySourceList.size() == 0)
    {
        return setStandbyOverride(newValue);
    }

    // Pass request to sources
    foreach (NodeBase* node, m_standbySourceList)
    {
        returnValue = returnValue && node->setStandbyOverrideRequest(sessionId, newValue);
    }

    // Revert changes if any source failed.
    if (newValue == true && returnValue == false)
    {
        foreach (NodeBase* node, m_standbySourceList)
        {
            node->setStandbyOverrideRequest(sessionId, false);
        }
    }

    return returnValue;
}
