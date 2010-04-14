TEMPLATE = lib
CONFIG += plugin

QT += dbus

TARGET = arianesensor

include( ../../common-config.pri )

DEFINES += NO_DBUS_FOR_ARIANE

SENSORFW_INCLUDEPATHS = ../.. \
    ../../include \
    ../../filters \
    ../../sensord \
    ../../datatypes \
    ../../adaptors

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += arianesensor.h \
    arianesensor_a.h \
    arianeplugin.h
SOURCES += arianesensor.cpp \
    arianesensor_a.cpp \
    arianeplugin.cpp

include(../../common-install.pri)
publicheaders.path = $${publicheaders.path}/arianeplugin
publicheaders.files += $$HEADERS

target.path = $$PLUGINPATH

INSTALLS += target
