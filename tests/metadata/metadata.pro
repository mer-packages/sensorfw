QT += testlib \
      dbus \
      network
QT -= gui

include(../common-install.pri)

CONFIG += debug
TEMPLATE = app
TARGET = sensormetadata-test
HEADERS += metadatatest.h
SOURCES += metadatatest.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../filters \
                        ../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../../qt-api -lsensorclient \
                      -L../../datatypes -lsensordatatypes
