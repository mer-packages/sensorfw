TEMPLATE     = lib
CONFIG      += plugin

TARGET       = touchadaptor

include( ../../common-config.pri )

HEADERS += touchadaptor.h \
           touchadaptorplugin.h

SOURCES += touchadaptor.cpp \
           touchadaptorplugin.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../ \
                        ../../sensord \
                        ../../datatypes \
                        ../../filters 

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
