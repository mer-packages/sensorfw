/**
   @file dummyclient.cpp
   @brief Helper client for automatic tests

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

#include <QCoreApplication>
#include "dummyclient.h"

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    DummyClient dc;
    
    QTimer::singleShot(1500, &dc, SLOT(closeSession()));
    QObject::connect(&dc, SIGNAL(sessionClosed()), &app, SLOT(quit()));

    return app.exec();
}
