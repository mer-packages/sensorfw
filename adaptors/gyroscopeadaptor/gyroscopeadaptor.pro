TEMPLATE     = lib
CONFIG      += plugin

TARGET       = gyroscopeadaptor

include( ../../common-config.pri )

HEADERS += gyroscopeadaptor.h \
           gyroscopeadaptorplugin.h

SOURCES += gyroscopeadaptor.cpp \
           gyroscopeadaptorplugin.cpp

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
