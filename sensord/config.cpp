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

static Config *static_configuration = 0;

Config::Config() : m_settings(NULL) {
}

Config::~Config() {
    delete m_settings;
}

Config *Config::loadConfig(const QString &configFileName) {
    Config *config = 0;
    QFile file(configFileName);
    if (file.open(QIODevice::ReadOnly))
      file.close();
    else
        return config;
    if (static_configuration) {
        config = static_configuration;
    } else {
        config = new Config();
        config->m_settings = new QSettings(configFileName, QSettings::IniFormat);
        static_configuration = config;
    }
    return config;
}

QVariant Config::value(const QString &key, const QVariant &defaultValue) const {
   return m_settings->value(key, defaultValue);
}

Config *Config::configuration() {
    if (!static_configuration) {
        // This shouldn't happen because the configuration is loaded
        // when sensord starts (and before any plugins are loaded)
        sensordLogW() << "Configuration has not been loaded";
    }
    return static_configuration;
}
