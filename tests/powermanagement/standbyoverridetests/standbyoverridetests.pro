QT += testlib \
      dbus \
      network
QT -= gui

include(../../common-install.pri)

CONFIG += debug
TEMPLATE = app
TARGET = sensorstandbyoverride-test
HEADERS += standbyoverridetests.h helpslot.h
SOURCES += standbyoverridetests.cpp

SENSORFW_INCLUDEPATHS = ../../.. \
                        ../../../qt-api \
                        ../../../include \
                        ../../../filters \
                        ../../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../../../qt-api -lsensorclient \
                      -L../../../datatypes -lsensordatatypes
