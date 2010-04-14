/**
   @file sensorfakeopen.h
   @brief Deprecated library for faking sensor handles through system calls
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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

#include <sys/types.h>
#include <stdarg.h>

// Doing this to prevent collision with real ioctl()
#define _SYS_IOCTL_H
#include <linux/input.h>
#include <asm-generic/ioctl.h>

/**
 * libsensorfakeopen is a library that allows replacing the real device
 * driver handles by fake ones for testing and debugging purposes. 
 * 
 * To use, set FAKEOPEN_PATHS to contain the full path for the device
 * driver handle to fake, full path to the handle to use instead, and a
 * description word/string to return with ioctl(EVIOCGNAME) request
 * (only required for input devices, otherwise unused)
 * 
 * Format of FAKEOPEN_PATHS is: path1,fakepath1,ioctlname1:path2,fakepath2,ioctlname2:...
 * 
 * To write into input device handles, one must push proper input device events.
 * See datafaker.h for helpfull utils.
 * 
 * FAKEOPEN_FDS is for internal use only, it should not be touched by hand.
 * For the curious, the format is: fd1=desc1:fd2=desc2:...
 */

extern "C"
{
    int open(const char *pathname, int flags, mode_t mode);
    int ioctl(int d, int request, ...);
    int close(int fd);
}

