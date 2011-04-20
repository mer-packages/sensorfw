QT += testlib \
      dbus \
      network
QT -= gui

include(../common-install.pri)

CONFIG += debug
TEMPLATE = app
TARGET = sensorapi-test
HEADERS += clientapitest.h
SOURCES += clientapitest.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../filters \
			../../qt-api

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../../qt-api -lsensorclient \
                      -L../../datatypes -lsensordatatypes

