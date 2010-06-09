TEMPLATE = lib
TARGET = sensordatatypes
QT += dbus

include( ../common-config.pri )

SENSORFW_INCLUDEPATHS = .. \
    ../filters

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += xyz.h \
    orientation.h \
    unsigned.h \
    magneticfield.h \
    compass.h \
    datarange.h

SOURCES += xyz.cpp \
    orientation.cpp \
    unsigned.cpp \
    compass.cpp

internal {
    HEADERS += tap.h
    SOURCES += tap.cpp
}

include(../common-install.pri)
publicheaders.path  = $${publicheaders.path}/datatypes
publicheaders.files = $$HEADERS
target.path = $$SHAREDLIBPATH
INSTALLS += target
