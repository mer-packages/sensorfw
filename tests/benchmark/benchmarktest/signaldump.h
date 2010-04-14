/**
   @file signaldump.h
   @brief Automatic tests helper utility

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

#ifndef SIGNALDUMP_H
#define SIGNALDUMP_H

#include <QObject>
#include <QTime>
#include <QFile>
#include <QDebug>
#include <QStringList>
#include "datatypes/unsigned.h"

class SignalDump : public QObject
{
    Q_OBJECT;
public:
    SignalDump(QObject *parent = 0) : QObject(parent), cnt(0),
    jiffiesS(0), jiffiesU(0) {}

    /// sample counter
    int cnt;

    /// CPU counters
    QTime jiffiesTime;
    int jiffiesS;
    int jiffiesU;

    /// Memory counters
    QList<int> memoryClean;
    QList<int> memoryDirty; 

    void recordMemUsage(int pid)
    {
        int memClean = 0;
        int memDirty = 0;
        readSmaps(pid, memClean, memDirty);

        memoryClean.append(memClean);
        memoryDirty.append(memDirty);
    }

    bool readSmaps(int pid, int& clean, int& dirty)
    {
        QFile smapsFile(QString("/proc/%1/smaps").arg(pid));

        if (!smapsFile.exists() || !smapsFile.open(QIODevice::ReadOnly)) {
            return false;
        }

        QByteArray line = smapsFile.readLine();
        while (!line.isEmpty()) {
            if (*(line.constData()) == 'P' || *(line.constData()) == 'S') {
                if (strncmp(line.constData(), "Private_Clean", 13) == 0) {
                    clean += atoi(QString(line).split(' ', QString::SkipEmptyParts).at(1).toLocal8Bit().constData());
                }
                else if (strncmp(line.constData(), "Private_Dirty", 13) == 0) {
                    dirty += atoi(QString(line).split(' ', QString::SkipEmptyParts).at(1).toLocal8Bit().constData());
                }
                else if (strncmp(line.constData(), "Swap", 4) == 0) {
                    dirty += atoi(QString(line).split(' ', QString::SkipEmptyParts).at(1).toLocal8Bit().constData());
                }
            }
            line.clear();
            line = smapsFile.readLine();
        }
        smapsFile.close();
        return true;
    }

    double getCpuUsage(int pid)
    {
        if (!jiffiesTime.isValid()) {
            jiffiesTime = QTime::currentTime();
            readJiffies(pid, &jiffiesS, &jiffiesU);
            return 0;
        }

        int js = 0;
        int ju = 0;
        QTime jtime = QTime::currentTime();

        readJiffies(pid, &js, &ju);

        double cpuUser = (js - jiffiesU) / (100*jiffiesTime.msecsTo(jtime)/1000.0);
        double cpuSystem = (ju - jiffiesS) / (100*jiffiesTime.msecsTo(jtime)/1000.0);
 
        jiffiesTime = jtime;
        jiffiesS = js;
        jiffiesU = ju;

        return cpuUser + cpuSystem;
    }

    bool readJiffies(int pid, int* js, int *ju)
    {
        QFile procFile(QString("/proc/%1/stat").arg(pid));

        if (!procFile.exists() || !procFile.open(QIODevice::ReadOnly)) {
            return false;
        }

        QList<QByteArray> values = procFile.readLine().split(' ');

        *js = atoi(values.at(13));
        *ju = atoi(values.at(14));

        procFile.close();
        return true;
    }

public Q_SLOTS:
    void counterSlot(const Unsigned&)
    {
        cnt++;
    }
};

#endif
