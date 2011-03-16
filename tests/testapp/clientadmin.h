/**
   @file clientadmin.h
   @brief base class for sensorhandlers

   <p>
   Copyright (C) 2011 Nokia Corporation

   @author Shenghua Liu <ext-shenghua.1.liu@nokia.com>

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
#ifndef CLIENTADMIN_H
#define CLIENTADMIN_H

#include <QObject>
#include <QList>
#include <QString>

#include "abstractsensorhandler.h"
#include "parser.h"
#include "config.h"
#include "logging.h"
#include "statprinter.h"

class ClientAdmin : public QObject
{
    Q_OBJECT
public:
    ClientAdmin(const Parser& parser, QObject *parent = 0);
    ~ClientAdmin();
    void runClients();

private:
    Parser parser;
    QList<AbstractSensorHandler*> handlers;
    StatPrinter* printer;
    static const char* CONFIG_FILE_PATH;

    void init();
};

#endif // CLIENTADMIN_H
