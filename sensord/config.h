/**
   @file config.h
   @brief Configuration handler

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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

/**
 * @brief Holds sensord's configuration. Configuration is read and parsed with
 * the QSettings class. Config is a singleton. That is, configuration is read once when sensord
 * starts and kept in memory until the sensord process finishes.
 */
class Config {
public:
    Config();
    virtual ~Config();

    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

    static Config *configuration();
    static Config *loadConfig(const QString &configFileName);

private:
    Config(const Config &c);
    Config& operator=(const Config &c);

    QSettings *m_settings;
};

#endif // SENSORD_CONFIG_H
