include(../common-install.pri)

TEMPLATE = app
TARGET = sensorexternal-test

CONFIG += testcase link_pkgconfig

PKGCONFIG += mlite5

HEADERS += externaltests.h
SOURCES += externaltests.cpp
