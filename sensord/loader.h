/**
   @file loader.h
   @brief Plugin loader

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Semi Malinen <semi.malinen@nokia.com

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

#ifndef LOADER_H
#define LOADER_H

#include <QString>
#include <QStringList>
#include "plugin.h"

class FilterBase;

class Loader {
public:
    static Loader& instance();

    bool loadPlugin(const QString& name, QString* errorMessage = 0);

private:
    Loader();
    Loader(const Loader&);
    Loader& operator=(const Loader&);

    bool loadPluginFile(const QString& name, QString *errorString);
    QString resolveRealPluginName(const QString& pluginName);

    QStringList        loadedPluginNames_;
    QStringList        newPluginNames_;
    QList<PluginBase*> newPlugins_;
};

#endif
