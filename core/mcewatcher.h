/**
   @file mcewatcher.h
   @brief MCE signal utility

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
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


#ifndef SENSORD_MCE_WATCHER_H
#define SENSORD_MCE_WATCHER_H

#include <QObject>
#include <QDBusInterface>
#include <QString>
#include <QtDBus/QtDBus>

#define MCE_SERVICE                     "com.nokia.mce"
#define MCE_SIGNAL_PATH                 "/com/nokia/mce/signal"
#define MCE_SIGNAL_IF                   "com.nokia.mce.signal"
#define MCE_DISPLAY_OFF_STRING                  "off"
#define MCE_PSM_STATE_SIG               "psm_state_ind"
#define MCE_DISPLAY_SIG                 "display_status_ind"

#define MCE_REQUEST_PATH                "/com/nokia/mce/request"
#define MCE_REQUEST_IF                  "com.nokia.mce.request"
#define MCE_DISPLAY_STATUS_GET          "get_display_status"
#define MCE_PSM_STATE_GET               "get_psm_state"

/**
 * Class for monitoring various MCE signals.
 */
class MceWatcher : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MceWatcher)

public:
    /**
     * Constructor.
     *
     * @param parent Parent object.
     */
    MceWatcher(QObject* parent = 0);

    /**
     * Get display state.
     *
     * @return display state.
     */
    bool displayEnabled() const;

    /**
     * Get powersave-mode state.
     *
     * @return powersave-mode state.
     */
    bool PSMEnabled() const;

signals:
    /**
     * Emitted when display state has changed.
     *
     * @param displayOn \c true if display went to 'on' or 'dimmed',
     *                  \c false if 'off'.
     */
    void displayStateChanged(bool displayOn);

    /**
     * Emitted when powersave-mode has changed.
     *
     * @param PSM is powersave-mode enabled or not.
     */
    void devicePSMStateChanged(bool PSM);

private slots:
    /**
     * Slot for MCE display state change signals.
     *
     * @param state name of the state.
     */
    void slotDisplayStateChanged(const QString& state);

    /**
     * Slot for MCE powersave-mode state change signals.
     *
     * @param mode is powersave-mode enabled or not.
     */
    void slotPSMStateChanged(bool mode);

    void displayStateReplyFinished(QDBusPendingCallWatcher *watch);

private:
    QDBusInterface* dbusIfc; /**< DBus iface to MCE */
    bool displayState;       /**< current display state */
    bool powerSave;          /**< current powersave-mode state */

};

#endif // SENSORD_MCE_WATCHER_H
