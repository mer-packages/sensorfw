TEMPLATE     = lib
CONFIG      += plugin

TARGET       = proximityadaptor-evdev

include( ../../common-config.pri )

HEADERS += proximityadaptor-evdev.h \
           proximityadaptor-evdevplugin.h

SOURCES += proximityadaptor-evdev.cpp \
           proximityadaptor-evdevplugin.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../sensord \
                        ../../datatypes \
                        ../../filters

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
