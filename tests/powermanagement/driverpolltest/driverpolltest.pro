QT = core \
     dbus \
     network
QT -= gui

include(../../common-install.pri)

CONFIG += debug
TEMPLATE = app
TARGET = sensordriverpoll-test
HEADERS += driverpolltest.h
SOURCES += driverpolltest.cpp

SENSORFW_INCLUDEPATHS = ../../../qt-api \
                        ../../../include \
                        ../../../filters \
                        ../../../datatypes \
                        ../../..

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../../../qt-api  \
                      -L../../../datatypes\
                      -L../../../core

equals(QT_MAJOR_VERSION, 4):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes -lsensorclient
}
equals(QT_MAJOR_VERSION, 5):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes-qt5 -lsensorclient-qt5
}
