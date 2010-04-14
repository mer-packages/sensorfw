TEMPLATE = lib
CONFIG += plugin
TARGET = arianeadaptor

include( ../../common-config.pri )

SENSORFW_INCLUDEPATHS = ../.. \
                        ../ \
                        ../../include \
                        ../../sensord \
                        ../../datatypes \
                        ../../filters
DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS
HEADERS = arianeentry.h \
    arianeadaptor.h \
    arianeadaptorplugin.h
SOURCES = arianeentry.cpp \
    arianeadaptor.cpp \
    arianeadaptorplugin.cpp

include(../../common-install.pri)
publicheaders.path = $${publicheaders.path}/arianeadaptor
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
