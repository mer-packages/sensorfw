TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus

TARGET       = gyroscopesensor

include( ../../common-config.pri )

HEADERS += gyroscopesensor.h   \
           gyroscopesensor_a.h \
           gyroscopeplugin.h

SOURCES += gyroscopesensor.cpp   \
           gyroscopesensor_a.cpp \
           gyroscopeplugin.cpp

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
