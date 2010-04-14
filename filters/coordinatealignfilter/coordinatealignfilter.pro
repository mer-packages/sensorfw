TEMPLATE = lib
CONFIG += plugin
QT -= gui
QT += dbus

TARGET = coordinatealignfilter

include( ../../common-config.pri )

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        .. \
                        ../../sensord \
                        ../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += coordinatealignfilter.h \
           coordinatealignfilterplugin.h

SOURCES += coordinatealignfilter.cpp \
           coordinatealignfilterplugin.cpp

include(../../common-install.pri)
publicheaders.files = $$HEADERS

target.path = $$PLUGINPATH
INSTALLS += target
