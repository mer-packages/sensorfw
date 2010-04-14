TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus network

TARGET       = proximitysensor

include( ../../common-config.pri )

HEADERS += proximitysensor.h   \
           proximitysensor_a.h \
           proximityplugin.h

SOURCES += proximitysensor.cpp   \
           proximitysensor_a.cpp \
           proximityplugin.cpp

SENSORFW_INCLUDEPATHS = ../..           \
                        ../../include   \
                        ../../sensord   \
                        ../../datatypes \
                        ../../filters
                        
DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
