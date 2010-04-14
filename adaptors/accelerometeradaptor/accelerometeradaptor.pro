TEMPLATE     = lib
CONFIG      += plugin

TARGET       = accelerometeradaptor

include( ../../common-config.pri )

HEADERS += accelerometeradaptor.h \
           accelerometeradaptorplugin.h

SOURCES += accelerometeradaptor.cpp \
           accelerometeradaptorplugin.cpp

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
