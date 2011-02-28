QT += core testlib dbus network
QT -= gui

CONFIG += debug
TEMPLATE = app
TARGET = sensortestapp

HEADERS += sensorhandler.h \
           parser.h \
           statprinter.h

SOURCES += main.cpp \
           sensorhandler.cpp \
           parser.cpp \
           statprinter.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../filters \
                        ../../datatypes \
                        ../../core

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../../qt-api -lsensorclient \
                      -L../../datatypes -lsensordatatypes \
                      -L../../core -lsensorfw

testconf.path = /usr/share/sensord-tests/
testconf.files = testapp.conf

target.path = /usr/bin

INSTALLS += target testconf
