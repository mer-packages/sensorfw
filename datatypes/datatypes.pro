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
    datarange.h \
    utils.h \
    timedunsigned.h \
    genericdata.h \
    orientationdata.h


SOURCES += xyz.cpp \
    orientation.cpp \
    unsigned.cpp \
    compass.cpp \
    utils.cpp


LIBS += -lrt


internal {
    HEADERS += tap.h \
               posedata.h \
               tapdata.h \
               touchdata.h

    SOURCES += tap.cpp
}

include(../common-install.pri)
publicheaders.path  = $${publicheaders.path}/datatypes
publicheaders.files = $$HEADERS
target.path = $$SHAREDLIBPATH
INSTALLS += target
