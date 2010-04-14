TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus

TARGET       = accelerometersensor

include( ../../common-config.pri )

HEADERS += accelerometersensor.h   \
           accelerometersensor_a.h \
           accelerometerplugin.h

SOURCES += accelerometersensor.cpp   \
           accelerometersensor_a.cpp \
           accelerometerplugin.cpp

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
