/**
   @file main.cpp
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
#include <QDebug>
#include <QFile>
#include <QStringList>
#include <linux/input.h>

void printUsage(QString name) 
{
    qDebug() << "Usage:" << name << "<file>";
    qDebug() << "  where <file> is a path to the filename to write to";
    qDebug() << "  and [delay] is optional minimum delay (msec) to wait between lines.";
    qDebug() << "";
    qDebug() << "  Input is read from stdin. Input should be a list of space separated";
    qDebug() << "  integers, with line change between single event sets.";
    qDebug() << "";
    qDebug() << "  Input structure: val1 val2 val3";
    qDebug() << "  Creates sequence:";
    qDebug() << "    EV_ABS:ABS_X:val1";
    qDebug() << "    EV_ABS:ABS_Y:val2";
    qDebug() << "    EV_ABS:ABS_Z:val3";
    qDebug() << "    EV_SYN:0:0";
    qDebug() << "";
    qDebug() << "  Empty line stops input.";
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printUsage(argv[0]);
        return -1;
    }

    //~ int delay = -1;
    //~ if (argc >=2) {
        //~ delay = QString(argv[2]).toInt();
    //~ }

    DataFaker dataFaker;

    if (!dataFaker.open(argv[1], "target")) {
        qDebug() << "Unable to open" << argv[1] << "for writing.";
        return -1;
    }

    QFile inputFile;
    if (!inputFile.open(stdin, QIODevice::ReadOnly)) {
        qWarning() << "Could not open STDIN for reading.";
    }

    char buffer[1024];
    while (!inputFile.atEnd()) {
        if (inputFile.readLine(buffer, sizeof(buffer)) <= 1) {
            break;
        }
        QString line(buffer);
        QStringList numberList = line.split(" ", QString::SkipEmptyParts);
        if (numberList.length() % 3 != 0) {
            qDebug() << "Too many / too few numbers on line, skipping:" << line;
        }

        for (int i = 0; i < numberList.length(); i+=3) {
            // Change coordinates temporarily for sensord-0.6.31
            dataFaker.write("target", EV_ABS, ABS_X, numberList.at(i+1).toInt());
            dataFaker.write("target", EV_ABS, ABS_Y, -numberList.at(i).toInt());
            dataFaker.write("target", EV_ABS, ABS_Z, numberList.at(i+2).toInt());
            dataFaker.write("target", EV_SYN, 0, 0);
        }
    }

    inputFile.close();
    dataFaker.close("target");

    return 0;
}
