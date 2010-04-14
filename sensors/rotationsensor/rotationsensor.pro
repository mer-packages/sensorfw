TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus network

TARGET       = rotationsensor

include( ../../common-config.pri )

HEADERS += rotationsensor.h   \
           rotationsensor_a.h \
           rotationplugin.h

SOURCES += rotationsensor.cpp   \
           rotationsensor_a.cpp \
           rotationplugin.cpp

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
