TEMPLATE     = lib
CONFIG      += plugin

TARGET       = tapadaptor

include( ../../common-config.pri )

HEADERS += tapadaptor.h \
           tapadaptorplugin.h

SOURCES += tapadaptor.cpp \
           tapadaptorplugin.cpp

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
