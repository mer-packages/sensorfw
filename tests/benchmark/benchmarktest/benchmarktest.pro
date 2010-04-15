QT += testlib \
      dbus \
      network
QT -= gui

include(../../common-install.pri)

CONFIG += debug
TEMPLATE = app
TARGET = sensorbenchmark-test
HEADERS += benchmarktests.h signaldump.h
SOURCES += benchmarktests.cpp

SENSORFW_INCLUDEPATHS = ../../.. \
                        ../../../include \
                        ../../../filters \
                        ../../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../../../qt-api -lsensorclient \
                      -L../../../datatypes -lsensordatatypes
