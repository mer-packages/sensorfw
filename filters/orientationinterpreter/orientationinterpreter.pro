TEMPLATE = lib
CONFIG += plugin

QT += dbus
QT -= gui

TARGET = orientationinterpreter

include( ../../common-config.pri )

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        .. \
                        ../../sensord \
                        ../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += orientationinterpreter.h \
           orientationinterpreterplugin.h

SOURCES += orientationinterpreter.cpp \
           orientationinterpreterplugin.cpp

include(../../common-install.pri)
publicheaders.files = $$HEADERS

target.path = $$PLUGINPATH
INSTALLS += target
