
include(../common-install.pri)

TEMPLATE = app
TARGET = sensorexternal-test

CONFIG += testcase link_pkgconfig

PKGCONFIG += gconf-2.0

HEADERS += externaltests.h
SOURCES += externaltests.cpp
