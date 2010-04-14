TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus network

TARGET       = magnetometersensor

include( ../../common-config.pri )

HEADERS += magnetometersensor.h   \
           magnetometersensor_a.h \
           magnetometerplugin.h

SOURCES += magnetometersensor.cpp   \
           magnetometersensor_a.cpp \
           magnetometerplugin.cpp

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
