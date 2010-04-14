QT += testlib \
    dbus \
    network
TEMPLATE = app
TARGET = client-example

include( ../../common-config.pri )

HEADERS += clientapitest.h \
    testclass.h
SOURCES += clientapitest.cpp
SENSORFW_INCLUDEPATHS = ../.. \
    ../../include \
    ../../filters \
    ../../datatypes
DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS
LIBS += -L../../qt-api \
    -lsensorclient \
    -L../../datatypes \
    -lsensordatatypes
ariane {
    DEFINES += USE_ARIANE
    HEADERS += gesturehandler.h
    SOURCES += gesturehandler.cpp
}
compass:DEFINES += USE_COMPASS
internal:DEFINES += USE_INTERNAL

#include(../../common-install.pri)
target.path = /usr/sbin/sensord-examples
INSTALLS += target
