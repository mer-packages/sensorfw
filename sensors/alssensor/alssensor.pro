TEMPLATE     = lib
CONFIG      += plugin link_pkgconfig

QT          += dbus network

TARGET       = alssensor

include( ../../common-config.pri )

HEADERS += alssensor.h   \
           alssensor_a.h \
           alsplugin.h

SOURCES += alssensor.cpp   \
           alssensor_a.cpp \
           alsplugin.cpp

SENSORFW_INCLUDEPATHS = ../..           \
                        ../../sensord   \
                        ../../datatypes \
                        ../../filters \
                        ../../include
                        
DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

contextprovider {
    DEFINES += PROVIDE_CONTEXT_INFO
    PKGCONFIG   += contextprovider-1.0
}

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target

