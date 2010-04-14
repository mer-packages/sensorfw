TEMPLATE = app
TARGET = sensordummyclient
QT += dbus network

include( ../../common-install.pri)

INCLUDEPATH += ../../../qt-api \
               ../../../sensord \
               ../../../ \
               ../../../include

SOURCES += dummyclient.cpp
HEADERS += dummyclient.h

LIBS += -L../../../qt-api -lsensorclient \
        -L../../../datatypes -lsensordatatypes
