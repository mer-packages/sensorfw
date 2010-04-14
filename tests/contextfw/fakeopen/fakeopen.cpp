/**
   @file fakeopen.cpp
   @brief Deprecated library for faking sensor handle.
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Marja Hassinen <ext-marja.2.hassinen@nokia.com>

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

#include "fakeopen.h"

#include <QString>
#include <QStringList>
#include <QDebug>

#include <dlfcn.h>

static void init (void) __attribute__ ((constructor));

typedef int (*real_open_func)(__const char *name, int flags, mode_t mode);

real_open_func real_open;

static void init (void)
{
    qDebug() << "Initing fake open";
    real_open = (real_open_func)dlsym(RTLD_NEXT, "open");
}

int open(const char *pathname, int flags, mode_t mode)
{
    qDebug() << "Fake open called" << QString(pathname);

    // See which files to fake and what are the fake files
    const char *realPathsC = getenv("FAKEOPEN_REAL_PATHS");
    if (!realPathsC) {
        return real_open(pathname, flags, mode);
    }
    const char *fakePathsC = getenv("FAKEOPEN_FAKE_PATHS");
    if (!fakePathsC) {
        return real_open(pathname, flags, mode);
    }

    QStringList realPaths = QString(realPathsC).split(":");
    QStringList fakePaths = QString(fakePathsC).split(":");

    int ix = realPaths.indexOf(QString(pathname));
    if (ix != -1 && ix < fakePaths.size()) {
        qDebug() << "Faking";
        return real_open(fakePaths[ix].toAscii().constData(), flags, mode);
    }
    return real_open(pathname, flags, mode);
}

