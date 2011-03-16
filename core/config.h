/**
   @file config.h
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

#ifndef SENSORD_CONFIG_H
#define SENSORD_CONFIG_H

#include <QString>
#include <QVariant>
#include <QSettings>
#include <QList>

/**
 * @brief Holds sensord's configuration. Configuration is read and parsed with
 * the QSettings class. Config is a singleton. That is, configuration is read once when sensord
 * starts and kept in memory until the sensord process finishes.
 */
class Config {
public:
    ~Config();

    QVariant value(const QString &key) const;
    QStringList groups() const;

    template<typename T>
    T value(const QString &key, const T &def = T()) const;

    bool exists(const QString &key) const;

    static Config *configuration();
    static bool loadConfig(const QString &defConfigPath, const QString &configDPath);
    static void close();

private:
    Config();
    Config(const Config &c);
    Config& operator=(const Config &c);

    bool loadConfigFile(const QString &configFileName);
    void clearConfig();

    QList<QSettings*> settings;
};

template<typename T>
T Config::value(const QString &key, const T &def) const
{
    QVariant val(value(key));
    if(!val.isValid())
        return def;
    return val.value<T>();
}

#endif // SENSORD_CONFIG_H
