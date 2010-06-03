TEMPLATE = lib
CONFIG += plugin

QT += dbus
QT -= gui

TARGET = faceinterpreter

include( ../../common-config.pri )

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        .. \
                        ../../sensord \
                        ../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += faceinterpreter.h \
           faceinterpreterplugin.h

SOURCES += faceinterpreter.cpp \
           faceinterpreterplugin.cpp

include(../../common-install.pri)
publicheaders.files = $$HEADERS

target.path = $$PLUGINPATH
INSTALLS += target
