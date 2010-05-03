/**
   @file externaltests.cpp
   @brief Automatic tests for external issues
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

#include "externaltests.h"
#include <gconf/gconf-client.h>
#include <QDebug>

void ExternalTest::testDeclinationCorrectionKey()
{
    QString gconfkey("/system/osso/location/magneticvariation");

    g_type_init();

    GConfClient *client = NULL;
    client = gconf_client_get_default();
    QVERIFY2(client, "Failed to initialise GConf client.");

    GError *error = NULL;
    GConfValue* value = gconf_client_get_without_default(client, gconfkey.toLocal8Bit(), &error);
    QVERIFY2(value, QString("Failed to read declination key from GConf (%1)").arg(gconfkey).toLocal8Bit());

    qDebug() << "Stored value for declination correction:" << gconf_value_get_int(value);

    g_object_unref(client);
}

QTEST_MAIN(ExternalTest)
