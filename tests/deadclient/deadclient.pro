
include(../common-install.pri)

TEMPLATE = app
TARGET = sensord-deadclient
DEPENDPATH += .

QT += network dbus
QT -= gui
CONFIG += sensord debug
DEFINES += ORIENTATION

INCLUDEPATH += ../.. \
               ../../include \
               ../../filters \
               ../../datatypes \
               ../../qt-api

LIBS += -L../../qt-api -lsensorclient \
        -L../../datatypes -lsensordatatypes

# Input
HEADERS += datareceiver.h testwindow.h
SOURCES += deadclient.cpp

deadclienttest.files = sensord-deadclienttest.py
deadclienttest.path = /usr/share/sensord-tests

INSTALLS += deadclienttest
