/**
   @file loader.cpp
   @brief Plugin loader

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Semi Malinen <semi.malinen@nokia.com
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

#include "loader.h"
#include "plugin.h"
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QHash>
#include <QCoreApplication>

#include "logging.h"
#include "config.h"

//namespace {
//    QHash<QString, Loader::FilterFactory> filterTypes;
//}

Loader::Loader()
{
}

Loader& Loader::instance()
{
    static Loader the_loader;

    return the_loader;
}

bool Loader::loadPluginFile(const QString& name, QString *errorString)
{
    bool    loaded = true;
    QString error;
    QDir    dir; // TODO

    sensordLogT() << "Loading plugin:" << name;
    
    // TODO: perhaps use QCoreApplication::addLibraryPath();
    // TODO: put the path definition to some reasonable place
    QCoreApplication::addLibraryPath("/usr/lib/sensord/");
    QLibrary ql(name);
    //QLibrary ql(dir.absoluteFilePath(name));
    // TODO: this means RTLD_GLOBAL to dlopen(); think twice before using:
    ql.setLoadHints(QLibrary::ExportExternalSymbolsHint);
    if (!ql.load()) {
        *errorString = ql.errorString();
        sensordLogC() << "plugin loading error: " << *errorString;
        return false;
    } 

    QtPluginInstanceFunction instance;
    instance = (QtPluginInstanceFunction)ql.resolve("qt_plugin_instance");
    if (!instance) {
        error = "qt_plugin_instance not found";
        sensordLogC() << "plugin loading error: " << error;
        return false;
    }
    
    QObject* object = instance();
    if (!object) {
        error = "not able to instanciate";
        sensordLogC() << "plugin loading error: " << error;
        return false;
    }
    
    PluginBase* plugin = qobject_cast<PluginBase*>(object);
    if (!plugin) {
        error = "not a Plugin type";
        sensordLogC() << "plugin loading error: " << error;
        return false;
    }

    newPluginNames_.append(name);
    newPlugins_.append(plugin);

    // Get dependencies
    QStringList requiredPlugins(plugin->Dependencies());
    sensordLogT() << name << " requires: " << requiredPlugins;

    for (int i = 0; i < requiredPlugins.size() && loaded; i++) {
        if (!(loadedPluginNames_.contains(requiredPlugins.at(i)) ||
              newPluginNames_.contains(requiredPlugins.at(i)))) 
        {
            sensordLogT() << requiredPlugins.at(i) << " is not yet loaded, trying to load.";
            QString resolvedName = resolveRealPluginName(requiredPlugins.at(i));
            sensordLogT() << requiredPlugins.at(i) << "resolved as" << resolvedName << ". Loading";
            loaded = loadPluginFile(resolvedName, &error);
        }
    }
    if (!loaded && errorString) {
        *errorString = error;
    }
    return loaded;
}

bool Loader::loadPlugin(const QString& name, QString* errorString)
{
    QString error;
    bool loaded = false;
    
    if (loadedPluginNames_.contains(name)) {
        sensordLogD() << "Plugin already loaded.";
        return true;
    }

    if (loadPluginFile(name, &error)) {
        
        // Register newly loaded plugins
        for (int i = newPlugins_.size()-1; i>= 0; i--) {
            (newPlugins_.at(i))->Register(*this);
        }

        // Init newly loaded plugins
        for (int i = newPlugins_.size()-1; i>= 0; i--) {
            (newPlugins_.at(i))->Init(*this);
        }

        loadedPluginNames_.append(newPluginNames_);
        loaded = true;

    } else {
        *errorString = error;
        loaded = false;
    }

    newPlugins_.clear();
    newPluginNames_.clear();

    return loaded;
}

QString Loader::resolveRealPluginName(const QString& pluginName)
{
    QString deviceId = Config::configuration()->value("deviceId", "default").toString();
    QString key = QString("%1/%2").arg(deviceId).arg(pluginName);

    QString nameFromConfig = Config::configuration()->value(key).toString();

    if (nameFromConfig == "") {
        sensordLogT() << "Plugin setting for " << pluginName << "not found from configuration. Using key as value.";
        return pluginName;
    }

    return nameFromConfig;
}
