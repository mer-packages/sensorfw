# -------------------------------------------------
# Project created by QtCreator 2009-04-06T14:56:43
# -------------------------------------------------
QT += dbus network
TARGET = sensorclient
TEMPLATE = lib

include( ../common-config.pri )

SOURCES += sensormanagerinterface.cpp \
    sensormanager_i.cpp \
    abstractsensor_i.cpp \
    socketreader.cpp
HEADERS += sensormanagerinterface.h \
    sensormanager_i.h \
    abstractsensor_i.h \
    socketreader.h
SENSORFW_INCLUDEPATHS = .. \
    ../include \
    ../filters \
    ../datatypes
DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

internal {
    SOURCES += compasssensor_i.cpp \
        orientationsensor_i.cpp \
        accelerometersensor_i.cpp \
        alssensor_i.cpp \
        tapsensor_i.cpp \
        proximitysensor_i.cpp \
        rotationsensor_i.cpp \
        magnetometersensor_i.cpp

    HEADERS += compasssensor_i.h \
        orientationsensor_i.h \
        accelerometersensor_i.h \
        alssensor_i.h \
        tapsensor_i.h \
        proximitysensor_i.h \
        rotationsensor_i.h \
        magnetometersensor_i.h
}

motionband {
    SOURCES += raworientationsensor_i.cpp
    HEADERS += raworientationsensor_i.h
    INCLUDEPATH += ../libs \
        ../libs/libfilter
}

ariane {
    SOURCES += filteredsensor_i.cpp \
        arianesensor_i.cpp
    HEADERS += filteredsensor_i.h \
        arianesensor_i.h
}

include(../common-install.pri)
publicheaders.files = $$HEADERS
target.path = $$SHAREDLIBPATH
INSTALLS += target
