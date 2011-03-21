/**
   @file idutils.h
   @brief ID utilities

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

#ifndef ID_UTILS_H
#define ID_UTILS_H

#include <QString>

/**
 * Return leftmost part separated by ";". For example return for "foo;bar" is "foo".
 *
 * @param id ID to clean.
 */
inline QString getCleanId(const QString& id)
{
    const char SENSOR_TYPE_SEPARATOR = ';';
    int pos = id.indexOf(SENSOR_TYPE_SEPARATOR);
    return pos != -1 ? id.left(pos) : id;
}

#endif // ID_UTILS_H
