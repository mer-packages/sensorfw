/**
   @file mcewatcher.cpp
   @brief MCE signal utility

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>

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


#include "mcewatcher.h"
#include <QDBusReply>

MceWatcher::MceWatcher(QObject* parent) : QObject(parent),
                                          dbusIfc(new QDBusInterface(MCE_SERVICE,
                                                                     MCE_SIGNAL_PATH,
                                                                     MCE_SIGNAL_IF,
                                                                     QDBusConnection::systemBus(),
                                                                     parent)),
                                          displayState(true),
                                          powerSave(false)
{
    dbusIfc->connection().connect(dbusIfc->service(),
                                  dbusIfc->path(),
                                  dbusIfc->interface(),
                                  MCE_PSM_STATE_SIG,
                                  this,
                                  SLOT(slotPSMStateChanged(bool)));

    dbusIfc->connection().connect(dbusIfc->service(),
                                  dbusIfc->path(),
                                  dbusIfc->interface(),
                                  MCE_DISPLAY_SIG,
                                  this,
                                  SLOT(slotDisplayStateChanged(const QString)));

// get the current state, instead of assuming it's true
    QDBusPendingReply<QString> displayStateReply = QDBusConnection::systemBus().asyncCall(
                QDBusMessage::createMethodCall(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF,
                                               MCE_DISPLAY_STATUS_GET));
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(displayStateReply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(displayStateReplyFinished(QDBusPendingCallWatcher*)));
}

void MceWatcher::displayStateReplyFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<QString> reply = *watch;

    if(reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        if (reply.value() == MCE_DISPLAY_OFF_STRING) {
            displayState = false;
        }
    }
}

void MceWatcher::slotDisplayStateChanged(const QString& state)
{
    bool newState = true;
    if (state == MCE_DISPLAY_OFF_STRING) {
        newState = false;
    }

    if (displayState != newState)
    {
        displayState = newState;
        emit displayStateChanged(displayState);
    }
}

void MceWatcher::slotPSMStateChanged(bool mode)
{
    if (powerSave != mode)
    {
        powerSave = mode;
        emit devicePSMStateChanged(powerSave);
    }
}

bool MceWatcher::displayEnabled() const
{
    return displayState;
}

bool MceWatcher::PSMEnabled() const
{
    return powerSave;
}
