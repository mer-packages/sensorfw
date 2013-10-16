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

SENSORFW_INCLUDEPATHS = ../../../include \
                        ../../../filters \
                        ../../../datatypes \
			../../../qt-api \
                        ../../../core \
                        ../../..

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../../../qt-api \
                      -L../../../datatypes \
                      -L../../../core

equals(QT_MAJOR_VERSION, 4):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes -lsensorclient
}
equals(QT_MAJOR_VERSION, 5):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes-qt5 -lsensorclient-qt5
}
