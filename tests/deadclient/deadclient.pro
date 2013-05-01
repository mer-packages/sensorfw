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

QMAKE_LIBDIR_FLAGS += -L../../qt-api  \
                      -L../../datatypes
equals(QT_MAJOR_VERSION, 4):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes -lsensorclient
}
equals(QT_MAJOR_VERSION, 5):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes-qt5 -lsensorclient-qt5
}

# Input
HEADERS += datareceiver.h testwindow.h
SOURCES += deadclient.cpp

deadclienttest.files = sensord-deadclienttest.py
deadclienttest.path = /usr/share/sensorfw-tests

INSTALLS += deadclienttest
