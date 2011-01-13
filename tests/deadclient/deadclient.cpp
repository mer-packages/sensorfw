/**
   @file deadclient.cpp
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
#include <QCoreApplication>
#include <QDebug>
#include <orientationsensor_i.h>
#include <accelerometersensor_i.h>
#include <compasssensor_i.h>
#include <rotationsensor_i.h>
#include <magnetometersensor_i.h>
#include <sensormanagerinterface.h>
#include <xyz.h>
#include "testwindow.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    if (argc > 1 && !strcmp(argv[1], "-c")) {
        control = true;
    }

    TestWindow t();
    
    return app.exec();
};
