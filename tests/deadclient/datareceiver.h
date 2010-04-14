/**
   @file deadclient.h
   @brief Automatic tests for dead client handling
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#include <QObject>
#include <xyz.h>

class DataReceiver : public QObject {
    Q_OBJECT;
public:
    DataReceiver(QObject* parent = 0) : QObject(parent) {}
    
public slots:
    void dump(const int& orientation) {
        qDebug() << __PRETTY_FUNCTION__ << orientation;
    }
    void dump(const XYZ& data) {
        qDebug() << __PRETTY_FUNCTION__ << data.x() << data.y() << data.z();
    }
};
