/**
   @file sfwerror.h
   @brief Error definitions

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

#ifndef SFW_ERROR_H
#define SFW_ERROR_H

static const int INVALID_SESSION = -1;

typedef enum
{
    SmNoError = 0,
    SmNotConnected,
    SmCanNotRegisterService,
    SmCanNotRegisterObject,
    SmAlreadyUnderControl,
    SmIdNotRegistered,
    SmFactoryNotRegistered,
    SmNotInstantiated,
    SmAdaptorNotStarted
} SensorManagerError;

typedef enum
{
    SNoError = 0,
    SSensorNotValid,
    SAdaptorNotStarted,
    SAdaptorNotObtained,
    SAdaptorNotInitialized,
    SHardwareIdNotInitialized,
    SRingBufferNotFound,
    SJoinFailed,
    SHwSensorStartFailed,
    SCannotAccessHwSensor,
    SPropertyGetError,
    SPropertySetError,
    SClientSocketError,
    //SaCannotAccessHwSensor,
    SaCannotAccessSensor,
    SaAccessNotAllowed,
    SaPropertyDoesNotExist,
    SaValueOutOfRange,
} SensorError;

#endif // SFW_ERROR_H
