QT += core testlib dbus network
QT -= gui


CONFIG += debug
TEMPLATE = app
TARGET = createsensors-test


HEADERS += testthread.h \
    parser.h

SOURCES += main.cpp \
           testthread.cpp \
    parser.cpp


SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../filters \
                        ../../datatypes \
                        ../../core

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../../qt-api -lsensorclient \
                      -L../../datatypes -lsensordatatypes

LIBS += -L../core -lsensorfw

testconf.path = /usr/share/sensord-tests/
testconf.files = createsensors.conf

target.path = /usr/bin

INSTALLS += target testconf
