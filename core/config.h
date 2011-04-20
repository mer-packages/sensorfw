/**
   @file config.h
   @brief Configuration handler

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
   @author Lihan Guo <ext-lihan.guo@nokia.com>
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

#ifndef SENSORD_CONFIG_H
#define SENSORD_CONFIG_H

#include <QString>
#include <QVariant>
#include <QSettings>
#include <QList>

/**
 * Sensord configuration parser. Configuration is read and parsed with
 * the QSettings class. Config is a singleton instance to which configuration
 * is loaded once during startup.
 */
class Config
{
public:
    /**
     * Destructor.
     */
    ~Config();

    /**
     * Find value for given key. Default QVariant is returned if key does
     * not exists.
     *
     * @param key Configuration key.
     * @return Value for given key.
     */
    QVariant value(const QString &key) const;

    /**
     * List of available groups in configuration.
     *
     * @return list of available configuration groups.
     */
    QStringList groups() const;

    /**
     * Find value for given key. Given default value is returned if key
     * does not exists.
     *
     * @tparam T Value type for configuration entry.
     * @param key Configuration key.
     * @param def Returned value if key does not exists.
     * @return Value for given key.
     */
    template<typename T>
    T value(const QString &key, const T &def = T()) const;

    /**
     * Does given key exists in configuration.
     *
     * @return does key exists in configuration.
     */
    bool exists(const QString &key) const;

    /**
     * Get configuration instance singleton. Object may not be deleted.
     *
     * @return configuration instance.
     */
    static Config *configuration();

    /**
     * Load configuration from given paths and append them to singleton
     * configuration instance.
     *
     * @param defConfigPath Path to the config file.
     * @param configDPath Path to the directory with config files.
     */
    static bool loadConfig(const QString &defConfigPath, const QString &configDPath);

    /**
     * Close singleton instance.
     */
    static void close();

private:
    /**
     * Constructor.
     */
    Config();

    /**
     * Copy constructor.
     *
     * @param c source object.
     */
    Config(const Config &c);

    /**
     * Assignment operator.
     *
     * @param c source object.
     * @return current object.
     */
    Config& operator=(const Config &c);

    /**
     * Load configuration file from given path.
     *
     * @param configFileName Configuration file path.
     * @return was configuration loaded successfully.
     */
    bool loadConfigFile(const QString &configFileName);

    /**
     * Clear configuration.
     */
    void clearConfig();

    QList<QSettings*> settings; /**< parsed QSettings */
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
