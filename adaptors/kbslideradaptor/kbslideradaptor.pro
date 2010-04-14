TEMPLATE     = lib
CONFIG      += plugin

TARGET       = kbslideradaptor

include( ../../common-config.pri )

HEADERS += kbslideradaptor.h \
           kbslideradaptorplugin.h

SOURCES += kbslideradaptor.cpp \
           kbslideradaptorplugin.cpp

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
