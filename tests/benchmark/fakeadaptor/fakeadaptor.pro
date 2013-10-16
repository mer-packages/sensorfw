TEMPLATE     = lib
CONFIG      += plugin

TARGET       = alsadaptor

include( ../../../common-config.pri )

HEADERS += fakeadaptor.h \
           fakeadaptorplugin.h

SOURCES += fakeadaptor.cpp \
           fakeadaptorplugin.cpp

SENSORFW_INCLUDEPATHS = ../../../include \
                        ../../../core \
                        ../../../datatypes \
                        ../../../filters \
                        ../../../core \
                        ../../..

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../../common-install.pri)
target.path = $$PLUGINPATH/testing

INSTALLS += target
