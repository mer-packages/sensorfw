TEMPLATE = lib
CONFIG += plugin

QT += dbus
QT -= gui

TARGET = rotationfilter

include( ../../common-config.pri )

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../sensord \
                        ../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += rotationfilter.h \
           rotationfilterplugin.h

SOURCES += rotationfilter.cpp \
           rotationfilterplugin.cpp

include(../../common-install.pri)
publicheaders.files = $$HEADERS

target.path = $$PLUGINPATH
INSTALLS += target
