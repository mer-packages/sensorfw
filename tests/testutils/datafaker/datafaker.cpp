/**
   @file datafaker.cpp
   @brief Data synthesizing utility
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

#include "datafaker.h"
#include <fcntl.h>
#include <linux/input.h>
#include <sys/time.h>
#include <QDebug>

DataFaker::DataFaker(QObject* parent) : QObject(parent) {}

DataFaker::~DataFaker() {
    cleanup();
}

bool DataFaker::open(QString path, QString name)
{
    if (m_fdMap.keys().contains(name)) {
        qDebug() << "[DataFaker]: Name" << name << "is already used by another fd.";
        return false;
    }

    int fd = ::open(path.toAscii().constData(), O_WRONLY);
    if (fd < 0) {
        qDebug() << "[DataFaker]: Failed to open" << path;
        return false;
    }

    m_fdMap.insert(name, fd);

    return true;
}

bool DataFaker::close(QString name)
{
    if (m_fdMap.keys().contains(name)) {
        ::close(m_fdMap.value(name));
        m_fdMap.remove(name);
    } else {
        qDebug() << "[DataFaker]: No fd associated with name" << name;
        return false;
    }
    return true;
}

bool DataFaker::write(QString name, const void *data, int size)
{
    if (!m_fdMap.keys().contains(name)) {
        qDebug() << "[DataFaker]: No fd associated with name" << name;
        return false;
    }

    if (::write(m_fdMap.value(name), data, size) < size) {
        qDebug() << "[DataFaker]: Wrong amount of bytes written to" << name;
        return false;
    }

    return true;
}

bool DataFaker::write(QString name, int type, int code, int value)
{
    struct input_event ev;
    ev.type = type;
    ev.code = code;
    ev.value = value;
    gettimeofday(&ev.time, 0);

    return write(name, (const void *)&ev, sizeof(ev));
}

void DataFaker::cleanup()
{
    foreach (QString key, m_fdMap.keys()) {
        close(key);
    }
}
