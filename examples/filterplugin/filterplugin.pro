TEMPLATE = lib
CONFIG += plugin

TARGET = samplefilter

include( ../../common-config.pri )

SENSORFW_INCLUDEPATHS = ../../include \
                        ../../sensord \
                        ../../core \
                        ../../datatypes \
                        ../..

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += samplefilter.h \
           samplefilterplugin.h

SOURCES += samplefilter.cpp \
           samplefilterplugin.cpp
