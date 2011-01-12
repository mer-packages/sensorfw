/**
   @file mcewatcher.cpp
   @brief MCE signal utility

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>

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


#include <mce/mode-names.h>
#include <mce/dbus-names.h>
#include "mcewatcher.h"

MceWatcher::MceWatcher(QObject* parent) : QObject(parent),
    dbusIfc(new QDBusInterface(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF,
                   QDBusConnection::systemBus(), parent)), m_state(true), m_powerSave(false)
{

    dbusIfc->connection().connect(dbusIfc->service(),
                                  dbusIfc->path(),
                                  dbusIfc->interface(),
                                  MCE_PSM_MODE_SIG,
                                  this,
                                  SLOT(slotPSMStateChanged(const bool)));

    dbusIfc->connection().connect(dbusIfc->service(),
                                  dbusIfc->path(),
                                  dbusIfc->interface(),
                                  MCE_DISPLAY_SIG,
                                  this,
                                  SLOT(slotDisplayStateChanged(const QString)));


}

void MceWatcher::slotDisplayStateChanged(const QString state)
{
    bool newState = true;
    if (state == MCE_DISPLAY_OFF_STRING) {
        newState = false;
    }

    if (m_state != newState)
    {
        m_state = newState;
        emit displayStateChanged(m_state);
    }
}


void MceWatcher::slotPSMStateChanged(const bool mode)
{
    if (m_powerSave != mode)
    {
        m_powerSave = mode;
        emit devicePSMStateChanged(m_powerSave);
    }

}
