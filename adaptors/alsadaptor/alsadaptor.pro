TEMPLATE     = lib
CONFIG      += plugin

TARGET       = alsadaptor

include( ../../common-config.pri )

HEADERS += alsadaptor.h \
           alsadaptorplugin.h

SOURCES += alsadaptor.cpp \
           alsadaptorplugin.cpp

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
