QT += core testlib dbus network
QT -= gui


CONFIG += debug
TEMPLATE = app
TARGET = createsensors-test


HEADERS += testthread.h

SOURCES += main.cpp \
           testthread.cpp

include(../common-install.pri)

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../filters \
                        ../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../../qt-api -lsensorclient \
                      -L../../datatypes -lsensordatatypes
