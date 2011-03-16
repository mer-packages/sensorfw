/**
   @file loader.cpp
   @brief Plugin loader

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Semi Malinen <semi.malinen@nokia.com
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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
#include <QLibrary>
#include <QStringList>
#include <QList>
#include <QCoreApplication>

#include "logging.h"
#include "config.h"

Loader::Loader()
{
    QCoreApplication::addLibraryPath("/usr/lib/sensord/");
}

Loader& Loader::instance()
{
    static Loader the_loader;

    return the_loader;
}

bool Loader::loadPluginFile(const QString& name, QString *errorString, QStringList& newPluginNames, QList<PluginBase*>& newPlugins) const
{
    sensordLogT() << "Loading plugin:" << name;

    QLibrary ql(name);
    ql.setLoadHints(QLibrary::ExportExternalSymbolsHint);
    if (!ql.load()) {
        *errorString = ql.errorString();
        sensordLogC() << "plugin loading error: " << *errorString;
        return false;
    }

    QtPluginInstanceFunction instance;
    instance = (QtPluginInstanceFunction)ql.resolve("qt_plugin_instance");
    if (!instance) {
        *errorString = "qt_plugin_instance not found";
        sensordLogC() << "plugin loading error: " << *errorString;
        return false;
    }

    QObject* object = instance();
    if (!object) {
        *errorString = "not able to instanciate";
        sensordLogC() << "plugin loading error: " << *errorString;
        return false;
    }

    PluginBase* plugin = qobject_cast<PluginBase*>(object);
    if (!plugin) {
        *errorString = "not a Plugin type";
        sensordLogC() << "plugin loading error: " << *errorString;
        return false;
    }

    // Add plugins to the front of the list so they are initialized in reverse order. This will guarantee that dependencies are initialized first for each plugin.
    newPluginNames.prepend(name);
    newPlugins.prepend(plugin);

    // Get dependencies
    QStringList requiredPlugins(plugin->Dependencies());
    sensordLogT() << name << " requires: " << requiredPlugins;

    bool loaded = true;
    for (int i = 0; i < requiredPlugins.size() && loaded; ++i) {
        if (!(loadedPluginNames_.contains(requiredPlugins.at(i)) ||
              newPluginNames.contains(requiredPlugins.at(i))))
        {
            sensordLogT() << requiredPlugins.at(i) << " is not yet loaded, trying to load.";
            QString resolvedName = resolveRealPluginName(requiredPlugins.at(i));
            sensordLogT() << requiredPlugins.at(i) << " resolved as " << resolvedName << ". Loading";
            loaded = loadPluginFile(resolvedName, errorString, newPluginNames, newPlugins);
        }
    }
    return loaded;
}

bool Loader::loadPlugin(const QString& name, QString* errorString)
{
    QString error;
    bool loaded = false;
    QStringList        newPluginNames;
    QList<PluginBase*> newPlugins;

    if (loadedPluginNames_.contains(name)) {
        sensordLogD() << "Plugin already loaded.";
        return true;
    }

    if (loadPluginFile(name, &error, newPluginNames, newPlugins)) {

        // Register newly loaded plugins
        foreach (PluginBase* base, newPlugins) {
            base->Register(*this);
        }
        loadedPluginNames_.append(newPluginNames);
        loaded = true;

        // Init newly loaded plugins
        foreach (PluginBase* base, newPlugins) {
            base->Init(*this);
        }

    } else {
        if(errorString)
            *errorString = error;
        loaded = false;
    }

    return loaded;
}

QString Loader::resolveRealPluginName(const QString& pluginName) const
{
    QString deviceId = Config::configuration()->value<QString>("deviceId", "default");
    QString key = QString("%1/%2").arg(deviceId).arg(pluginName);

    QString nameFromConfig = Config::configuration()->value(key).toString();

    if (nameFromConfig == "") {
        sensordLogT() << "Plugin setting for " << pluginName << " not found from configuration. Using key as value.";
        return pluginName;
    }

    return nameFromConfig;
}
