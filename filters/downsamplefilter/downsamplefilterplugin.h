/**
   @file downsamplefilterplugin.h
   @brief Plugin for DownsampleFilter

   <p>
   Copyright (C) 2011 Nokia Corporation

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

#ifndef DOWNSAMPLEFILTERPLUGIN_H
#define DOWNSAMPLEFILTERPLUGIN_H

#include "plugin.h"

class DownsampleFilterPlugin : public Plugin
{
private:
    void Register(class Loader& l);
};

#endif
