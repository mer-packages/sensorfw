/**
   @file parameterparser.h
   @brief Parse for sensor parameters

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>

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

#ifndef PARAMETERPARSER_H
#define PARAMETERPARSER_H

#include <QMap>
#include <QString>

class ParameterParser
{
public:
    static QMap<QString, QString> getPropertyMap(const QString& id);
    static void applyPropertyMap(QObject* object, const QMap<QString, QString> & propertyMap);

private:
    static const char TYPE_SEPARATOR            = ';';
    static const char PROP_STRING_SEPARATOR     = ',';
    static const char PROP_NAME_VALUE_SEPARATOR = '=';
};

#endif // PARAMETERPARSER_H
