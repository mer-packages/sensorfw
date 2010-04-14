TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus network

TARGET       = tapsensor

include( ../../common-config.pri )

HEADERS += tapsensor.h   \
           tapsensor_a.h \
           tapplugin.h

SOURCES += tapsensor.cpp   \
           tapsensor_a.cpp \
           tapplugin.cpp

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

