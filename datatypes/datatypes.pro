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
    compass.h

SOURCES += xyz.cpp \
    orientation.cpp \
    unsigned.cpp \
    compass.cpp

internal {
    HEADERS += tap.h
    SOURCES += tap.cpp
}

ariane {
    HEADERS += key.h
    SOURCES += key.cpp
}

motionband {
    HEADERS += raworientation.h
    SOURCES += raworientation.cpp
    INCLUDEPATH += ../libs
}

include(../common-install.pri)
publicheaders.path  = $${publicheaders.path}/datatypes
publicheaders.files = $$HEADERS
target.path = $$SHAREDLIBPATH
INSTALLS += target
