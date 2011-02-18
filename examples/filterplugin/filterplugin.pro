TEMPLATE = lib
CONFIG += plugin
QT -= gui
QT += dbus

TARGET = samplefilter

include( ../../common-config.pri )

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        .. \
                        ../../sensord \
                        ../../core \
                        ../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += samplefilter.h \
           samplefilterplugin.h

SOURCES += samplefilter.cpp \
           samplefilterplugin.cpp

include(../../common-install.pri)
publicheaders.files = $$HEADERS
target.path = $$PLUGINPATH
