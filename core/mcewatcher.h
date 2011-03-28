/**
   @file mcewatcher.h
   @brief MCE signal utility

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <lihan.guo@digia.com>
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

/**
 * Class for monitoring various MCE signals.
 */
class MceWatcher : public QObject
{
    Q_OBJECT;

public:
    /**
     * Constructor.
     *
     * @param parent Parent object.
     */
    MceWatcher(QObject* parent = 0);

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

private:
    QDBusInterface* dbusIfc; /**< DBus iface to MCE */
    bool displayState;       /**< current display state */
    bool powerSave;          /**< current powersave-mode state */

};

#endif // SENSORD_MCE_WATCHER_H
