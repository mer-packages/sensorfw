TEMPLATE = lib
CONFIG += plugin
QT -= gui
QT += dbus

TARGET = magnetometerscalefilter

include( ../../common-config.pri )

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        .. \
                        ../../sensord \
                        ../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += magnetometerscalefilter.h \
           magnetometerscalefilterplugin.h

SOURCES += magnetometerscalefilter.cpp \
           magnetometerscalefilterplugin.cpp

include(../../common-install.pri)
publicheaders.files = $$HEADERS

target.path = $$PLUGINPATH
INSTALLS += target
