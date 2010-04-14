TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus network

TARGET       = orientationsensor

include( ../../common-config.pri )

HEADERS += orientationsensor.h   \
           orientationsensor_a.h \
           orientationplugin.h

SOURCES += orientationsensor.cpp   \
           orientationsensor_a.cpp \
           orientationplugin.cpp

SENSORFW_INCLUDEPATHS = ../..           \
                        ../../include \
                        ../../sensord   \
                        ../../datatypes \
                        ../../filters
                        
DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
