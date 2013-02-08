include(../common-install.pri)

TEMPLATE = app
TARGET = sensord-deadclient

QT += network dbus
QT -= gui
CONFIG += debug
DEFINES += ORIENTATION

INCLUDEPATH += ../../ \
               ../../include \
               ../../filters \
               ../../qt-api

QMAKE_LIBDIR_FLAGS += -L../../qt-api -lsensorclient \
                      -L../../datatypes -lsensordatatypes

# Input
HEADERS += datareceiver.h testwindow.h
SOURCES += deadclient.cpp

deadclienttest.files = sensord-deadclienttest.py
deadclienttest.path = /usr/share/sensorfw-tests

INSTALLS += deadclienttest
