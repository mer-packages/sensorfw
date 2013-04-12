/**
   @file sensorfakeopen.cpp
   @brief Deprecated library for faking sensor handles through system calls
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Tapio Rantala <ext-tapio.rantala@nokia.com>

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

#include "sensorfakeopen.h"

#include <QString>
#include <QStringList>
#include <QDebug>

#include <dlfcn.h>

static void init (void) __attribute__ ((constructor));

typedef int (*real_open_func)(__const char *name, int flags, mode_t mode);
typedef int (*real_ioctl_func)(int d, int request, ...);
typedef int (*real_close_func)(int fd);

real_open_func real_open;
real_ioctl_func real_ioctl;
real_close_func real_close;

static void init (void)
{
    qDebug() << "[FAKEOPEN]: Initializing.";
    real_open = (real_open_func)dlsym(RTLD_NEXT, "open");
    real_ioctl = (real_ioctl_func)dlsym(RTLD_NEXT, "ioctl");
    real_close = (real_close_func)dlsym(RTLD_NEXT, "close");
}

int open(const char *pathname, int flags, mode_t mode)
{
    const char* fakedPathsC = getenv("FAKEOPEN_PATHS");
    if (!fakedPathsC) {
        return real_open(pathname, flags, mode);
    }

    QStringList fakedPaths = QString(fakedPathsC).split(":");

    foreach (QString pathString, fakedPaths) {
        QStringList pathList = pathString.split(",");
        if (pathList.at(0) == QString(pathname)) {
            qDebug() << "[FAKEOPEN]: Opening" << pathList.at(1) << "instead of" << pathList.at(0);
            
            int fd = real_open(pathList.at(1).toLatin1().constData(), flags, mode);
            if (fd >= 0) {
                const char* fakedFdsC = getenv("FAKEOPEN_FDS");
                QString fakedFds(fakedFdsC);
                fakedFds.append(QString(":%1=%2").arg(fd).arg(pathList.at(2)));

                if (setenv("FAKEOPEN_FDS", fakedFds.toLatin1().constData(), 1)) {
                    qDebug() << "[FAKEOPEN]: Setting FAKEOPEN_FDS failed!";
                }
            } else {
                qDebug() << "[FAKEOPEN]: Failed to open" << pathList.at(1) << "instead of" << pathList.at(0);
            }
            return fd;
        }
    }

    return real_open(pathname, flags, mode);
}

int close(int fd)
{
    const char* fakedFdsC = getenv("FAKEOPEN_FDS");
    if (!fakedFdsC) {
        return real_close(fd);
    }

    QStringList fakedFds = QString(fakedFdsC).split(":");

    for (int i = 0; i < fakedFds.size(); i++) {
        if (fakedFds.at(i).split("=").at(0).toInt() == fd) {
            qDebug() << "[FAKEOPEN]: Closing faked fd:" << fd;
            fakedFds.removeAt(i);
            if (setenv("FAKEOPEN_FDS", fakedFds.join(":").toLocal8Bit().constData(), 1)) {
                qDebug() << "[FAKEOPEN]: Setting FAKEOPEN_FDS failed!";
            }
        }
    }

    return real_close(fd);
}

int ioctl(int d, int request, ...)
{
    char* argp;
    va_list ap;

    va_start(ap, request);
    argp = va_arg(ap, char*);
    va_end(ap);

    const char* fakedFdsC = getenv("FAKEOPEN_FDS");

    if (fakedFdsC && _IOC_TYPE(request) == _IOC_TYPE(EVIOCGNAME(0))) {

        QStringList fakedFds = QString(fakedFdsC).split(":");
        foreach (QString fakedFd, fakedFds) {
            if (fakedFd.split("=").at(0).toInt() == d) {
                qDebug() << "[FAKEOPEN]: Faking ioctl() call ";
                sprintf(argp, "%s", fakedFd.split("=").at(1).toLatin1().constData());
                return 0;
            }
        }
    }
    return real_ioctl(d, request, argp);
}
