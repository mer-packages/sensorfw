/**
   @file helpslot.h
   @brief Helper utility for power management tests
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

#ifndef HELPSLOT_H
#define HELPSLOT_H

#include <stdlib.h>
#include <QDebug>
#include <QThread>
#include <QProcess>
#include "xyz.h"

/**
 * Helper class for data faking - must nudge acc handle if no sensor
 * is started on sensord start (UI does not start, no contextfw)
 */
class HelpSlot : public QThread
{
    Q_OBJECT;

public:
    HelpSlot(QString inputFile = "") : m_inputFile(inputFile), m_valueCount(0), m_stop(false) {}

    void setInputFile(QString inputFile) { m_inputFile = inputFile; }
    void reset() { m_valueCount = 0; }
    void stop() { m_stop = true; }

    void run()
    {
        if (!(m_inputFile == ""))
        {
            while(!m_stop) {
                msleep(250);
                system(QString("echo 0 0 0 | datafaker %1 & { sleep 0.1; eval 'kill $!' &> /dev/null; }").arg(m_inputFile).toLatin1().constData());
            }
        }
    }

    QString m_inputFile;
    int m_valueCount;
    bool m_stop;

public slots:
    void dataAvailable(const XYZ& data)
    {
        Q_UNUSED(data);
        m_valueCount++;
    }
};

#endif
