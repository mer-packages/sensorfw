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
#include <MGConfItem>
#include <QDebug>

void ExternalTest::testDeclinationCorrectionKey()
{
    QString gconfkey("/system/osso/location/settings/magneticvariation");

    MGConfItem item(gconfkey);
    QVariant value = item.value();
    QVERIFY2(value.isValid(), "Failed to read declination key");

    qDebug() << "Stored value for declination correction:" << value.toInt();
}

QTEST_MAIN(ExternalTest)
