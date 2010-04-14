/**
   @file propertyhandler.cpp
   @brief Handler for stacked properties

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

#include <QDebug>
#include <QStringList>
#include "propertyhandler.h"
#include "logging.h"

void PropertyHandler::setRequest(QString property, QString adaptor, int sessionId, int value)
{
    QString key = QString("%1::%2").arg(property).arg(adaptor);

    if (m_propertyMap.contains(key)) {
        m_propertyMap[key].insert(sessionId, value);
    } else {
        QMap<int, int> valueMap;
        valueMap.insert(sessionId, value);
        m_propertyMap.insert(key, valueMap);
    }
    sensordLogD() << "Setting request for " << property << "for" << adaptor << "from" << sessionId << "to" << value;
    
    // Notify SensorManager that a new request was received.
    emit propertyRequestReceived(property, adaptor);
}

void PropertyHandler::clearRequests(int sessionId)
{
    sensordLogD() << "Clearing requests from session:" << sessionId;

    foreach (QString key, m_propertyMap.keys()) {
        if (m_propertyMap[key].contains(sessionId)) {
            m_propertyMap[key].remove(sessionId);
            QStringList str(key.split("::"));
            
            // Notify SensorManager that a request has disappeared.
            emit propertyRequestReceived(str.at(0), str.at(1));
        }
    }
}

int PropertyHandler::getHighestValue(QString property, QString adaptor)
{
    int highestValue = -1;
    QMap<int, int> valueMap = m_propertyMap.value(QString("%1::%2").arg(property).arg(adaptor));

    foreach (int value, valueMap) {
        if ((value < highestValue || highestValue < 0) && value > 0) {
            highestValue = value;
        }
    }

    return highestValue > 0 ? highestValue : 0;
}
