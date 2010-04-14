TEMPLATE = lib
CONFIG += plugin

QT += dbus

TARGET = filteredsensor

include( ../../common-config.pri )

DEFINES += NO_DBUS_FOR_ARIANE

SENSORFW_INCLUDEPATHS = ../.. \
    ../../include \
    ../../filters \
    ../../sensord \
    ../../datatypes \
    ../../adaptors

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += filteredsensor.h \
    filteredsensor_a.h \
    filteredplugin.h
SOURCES += filteredsensor.cpp \
    filteredsensor_a.cpp \
    filteredplugin.cpp

include(../../common-install.pri)
publicheaders.path = $${publicheaders.path}/filteredplugin
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
