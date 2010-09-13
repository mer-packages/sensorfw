/**
   @file config.cpp
   @brief Configuration handler

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
   @author Lihan Guo <ext-lihan.guo@nokia.com>

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

#include "config.h"

#include <sensord/logging.h>
#include <QSettings>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QList>

static Config *static_configuration = 0;

Config::Config() {
}

Config::~Config() {
    clearConfig();
}

void Config::clearConfig() {
    QListIterator<QSettings*> i(settings);

    while(i.hasNext())
        delete i.next();
}

Config *Config::loadConfig(const QString &defConfigPath, const QString &configDPath) {
    Config *config = NULL;

    /* Check/create new static config */
    if (static_configuration) {
        config = static_configuration;
    } else {
        config = new Config();
    }

    /* Delete old configuration */
    config->clearConfig();

    /* Scan config.d dir */
    QDir dir(configDPath, "*.conf", QDir::Name, QDir::Files);
    QStringList fileList = dir.entryList();


    /* Load all conf files */
    config->loadConfigFile(defConfigPath);
    QStringListIterator i(fileList);
    while(i.hasNext())
        config->loadConfigFile(i.next());

    static_configuration = config;

    return config;
}

bool Config::loadConfigFile(const QString &configFileName) {
    QFile file(configFileName);

    /* Test if the file is readable */
    if (file.open(QIODevice::ReadOnly)) {
      file.close();
    }
    else
    {
        sensordLogW() << "Unable to open \"" << configFileName <<  "\", check permissions";
        return false;
    }

    settings.append(new QSettings(configFileName, QSettings::IniFormat));
    sensordLogD() << "Config file \"" << configFileName << "\" successfully loaded";
    return true;
}

QVariant Config::value(const QString &key, const QVariant &defaultValue) const {
    /* Iterate through configs so that keys in the first files
     * have preference over the last.
     */
    QListIterator<QSettings*> i(settings);
    while(i.hasNext())
    {
        if(i.next()->contains(key))
            return i.peekPrevious()->value(key, defaultValue);
    }

    return defaultValue;
}

Config *Config::configuration() {
    if (!static_configuration) {
        // This shouldn't happen because the configuration is loaded
        // when sensord starts (and before any plugins are loaded)
        sensordLogW() << "Configuration has not been loaded";
    }
    return static_configuration;
}

void Config::close() {
    delete static_configuration;
    static_configuration = 0;
}
