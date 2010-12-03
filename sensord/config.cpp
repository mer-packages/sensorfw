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
    foreach(QSettings* setting, settings)
        delete setting;
    settings.clear();
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
    foreach(QString file, fileList)
        config->loadConfigFile(dir.absoluteFilePath(file));

    static_configuration = config;

    return config;
}

bool Config::loadConfigFile(const QString &configFileName) {
    QSettings* setting = new QSettings(configFileName, QSettings::IniFormat);
    if(setting->status() == QSettings::NoError) {
        settings.append(setting);
        sensordLogD() << "Config file \"" << configFileName << "\" successfully loaded";
        return true;
    }
    else if(setting->status() == QSettings::AccessError)
        sensordLogW() << "Unable to open \"" << configFileName <<  "\" configuration file";
    else if(setting->status() == QSettings::FormatError)
        sensordLogW() << "Configuration file \"" << configFileName <<  "\" is in wrong format";
    else
        sensordLogW() << "Configuration file \"" << configFileName <<  "\" parsing failed to unknown error: " << setting->status();
    delete setting;
    return false;
}

QVariant Config::value(const QString &key, const QVariant &defaultValue) const {
    /* Iterate through configs so that keys in the first files
     * have preference over the last.
     */
    foreach(QSettings* setting, settings) {
        if(setting->contains(key))
            return setting->value(key, defaultValue);
    }
    return defaultValue;
}

Config *Config::configuration() {
    if (!static_configuration) {
        sensordLogW() << "Configuration has not been loaded";
    }
    return static_configuration;
}

void Config::close() {
    delete static_configuration;
    static_configuration = 0;
}
