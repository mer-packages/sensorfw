/**
   @file mcewatcher.h
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


#ifndef SENSORD_MCE_WATCHER_H
#define SENSORD_MCE_WATCHER_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>

/**
 * Class for monitoring MCE signals. Currently only monitors the
 * display state changed.
 */
class MceWatcher : public QObject {

    Q_OBJECT;

public:
    MceWatcher(QObject* parent = 0);

signals:
    /**
     * Sent when display state has changed.
     * @param displayOn \c true if display went to 'on' or 'dimmed',
     *                  \c false if 'off'.
     */
    void displayStateChanged(const bool displayOn);
    void devicePSMStateChanged(const bool PSM);

private slots:
    void slotDisplayStateChanged(const QString state);
    void slotPSMStateChanged(const bool mode);

private:
    QDBusInterface* dbusIfc;
    bool m_state;
    bool m_powerSave;

};

#endif // SENSORD_MCE_WATCHER_H
